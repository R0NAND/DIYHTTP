#include <stdio.h>
#include <string.h>

#include "http_enums.h"
#include "parsing.h"
#include "headers.h"
#include "util.h"

static Methods getMethod(const char *str){
  if(strcmp(str, "GET") == 0){
    return GET;
  }else if(strcmp(str, "POST") == 0){
    return POST;
  }else if(strcmp(str, "PUT") == 0){
    return PUT;
  }else if(strcmp(str, "PATCH") == 0){
    return PATCH;
  }else if(strcmp(str, "DELETE") == 0){
    return DELETE;
  }else if(strcmp(str, "HEAD") == 0){
    return HEAD;
  }else if(strcmp(str, "OPTIONS") == 0){
    return OPTIONS;
  }else{
    return BAD;
  }
};

static const unsigned char valid_token_char[256] = {
    [0 ... 255] = 0,  // default invalid

    // digits 0-9
    ['0'] = 1, ['1'] = 1, ['2'] = 1, ['3'] = 1, ['4'] = 1,
    ['5'] = 1, ['6'] = 1, ['7'] = 1, ['8'] = 1, ['9'] = 1,

    // uppercase letters
    ['A'] = 1, ['B'] = 1, ['C'] = 1, ['D'] = 1, ['E'] = 1, ['F'] = 1,
    ['G'] = 1, ['H'] = 1, ['I'] = 1, ['J'] = 1, ['K'] = 1, ['L'] = 1,
    ['M'] = 1, ['N'] = 1, ['O'] = 1, ['P'] = 1, ['Q'] = 1, ['R'] = 1,
    ['S'] = 1, ['T'] = 1, ['U'] = 1, ['V'] = 1, ['W'] = 1, ['X'] = 1,
    ['Y'] = 1, ['Z'] = 1,

    // lowercase letters
    ['a'] = 1, ['b'] = 1, ['c'] = 1, ['d'] = 1, ['e'] = 1, ['f'] = 1,
    ['g'] = 1, ['h'] = 1, ['i'] = 1, ['j'] = 1, ['k'] = 1, ['l'] = 1,
    ['m'] = 1, ['n'] = 1, ['o'] = 1, ['p'] = 1, ['q'] = 1, ['r'] = 1,
    ['s'] = 1, ['t'] = 1, ['u'] = 1, ['v'] = 1, ['w'] = 1, ['x'] = 1,
    ['y'] = 1, ['z'] = 1,

    // other allowed token chars from RFC 7230
    ['!'] = 1, ['#'] = 1, ['$'] = 1, ['%'] = 1, ['&'] = 1, ['\''] = 1,
    ['*'] = 1, ['+'] = 1, ['-'] = 1, ['.'] = 1, ['^'] = 1, ['_'] = 1,
    ['`'] = 1, ['|'] = 1, ['~'] = 1,
};

static const unsigned char printable_char[256] = {
    [0 ... 255] = 1,  // default: assume valid

    // control characters (0x00–0x1F) — invalid except tab (0x09)
    [0x00] = 0, [0x01] = 0, [0x02] = 0, [0x03] = 0, [0x04] = 0,
    [0x05] = 0, [0x06] = 0, [0x07] = 0, [0x08] = 0,
    /* tab (0x09) is allowed */
    [0x0A] = 0, [0x0B] = 0, [0x0C] = 0, [0x0D] = 0,
    [0x0E] = 0, [0x0F] = 0, [0x10] = 0, [0x11] = 0,
    [0x12] = 0, [0x13] = 0, [0x14] = 0, [0x15] = 0,
    [0x16] = 0, [0x17] = 0, [0x18] = 0, [0x19] = 0,
    [0x1A] = 0, [0x1B] = 0, [0x1C] = 0, [0x1D] = 0,
    [0x1E] = 0, [0x1F] = 0,

    // delete (0x7F)
    [0x7F] = 0,
};

typedef enum {
    REQUEST_LINE,
    HEADER_LINES,
    BODY,
} ParserState;

static const char CRLF[] = "\r\n";

int parse_http_request(char *request, size_t reqLen, RequestLine *reqLine, HeaderMap *headers, size_t headersLen){
    ParserState state = REQUEST_LINE;
    if(0 == reqLen){
        return -1;
    }    
    char *reqEnd = request + reqLen;  
    char *method = NULL,    *resource = NULL,    *version = NULL,
         *methodEnd = NULL, *resourceEnd = NULL, *versionEnd = NULL;
    char *hdrName = NULL,    *hdrValue = NULL,
         *hdrNameEnd = NULL, *hdrValueEnd = NULL;
    int hasHostHdr = 1;
    
    for(char *cursor = request; cursor < reqEnd; cursor++){
       switch(state){
           case REQUEST_LINE:
               if(method == NULL){
                   if(valid_token_char[*cursor]){
                       method = cursor;
                   }else{
                       return BAD_REQUEST;
                   }
               }else if(methodEnd == NULL){
                   if(!valid_token_char[*cursor]){
                       if(*cursor == ' '){
                           methodEnd = cursor - 1;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(resource == NULL){
                   if(valid_token_char[*cursor]){
                       resource = cursor; 
                   }else{
                       if(*cursor != ' '){
                           return BAD_REQUEST;
                       }
                   }
               }else if(resourceEnd == NULL){
                   if(!valid_token_char[*cursor]){
                       if(*cursor == ' '){
                           resourceEnd = cursor - 1;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(version == NULL){
                   if(valid_token_char[*cursor]){
                       version = cursor; 
                   }else{
                       if(*cursor != ' '){
                           return BAD_REQUEST;
                       }
                   }
               }else if(versionEnd == NULL){
                   if(!valid_token_char[*cursor]){
                       if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                           versionEnd = cursor - 1;
                           state = HEADER_LINES;
                           cursor++;

                       }else if(*cursor == ' '){
                           versionEnd = cursor - 1;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else{
                   if(*cursor != ' '){
                       if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                           state = HEADER_LINES;
                           cursor++;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }
               break;
           case HEADER_LINES:
               if(hdrName == NULL){
                   if(valid_token_char[*cursor]){
                       hdrName = cursor;
                   }else if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1] && hasHostHdr){
                       cursor++;
                       state = BODY;
                   }else{
                       return BAD_REQUEST;
                   }
               }else if(hdrNameEnd == NULL){
                   if(*cursor == ':'){
                       hdrNameEnd = cursor - 1;
                   }else if(!valid_token_char[*cursor]){
                       return BAD_REQUEST;
                   }
               }else if(hdrValue == NULL){
                   if(*cursor != ' '){
                       if(printable_char[*cursor]){
                           hdrValue = cursor;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(hdrValueEnd == NULL){
                   if(!printable_char[*cursor]){
                       if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                           hdrValueEnd = cursor - 1;
                           cursor++;
                           //TODO: ASSIGN HEADER. REMOVE TRAILING SPACE.
                           hdrName = hdrNameEnd = hdrValue = hdrValueEnd = NULL;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }
               break;
           case BODY:
               break;
       }
    } 
    return 200;
}
