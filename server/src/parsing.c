#include <stdio.h>
#include <string.h>

#include "http_enums.h"
#include "parsing.h"
#include "headers.h"
#include "span_string.h"
#include "util.h"

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
    [0 ... 255] = 1,

    // control characters (0x00–0x1F)
    [0x00] = 0, [0x01] = 0, [0x02] = 0, [0x03] = 0, [0x04] = 0,
    [0x05] = 0, [0x06] = 0, [0x07] = 0, [0x08] = 0, [0x09] = 0,
    [0x0A] = 0, [0x0B] = 0, [0x0C] = 0, [0x0D] = 0,
    [0x0E] = 0, [0x0F] = 0, [0x10] = 0, [0x11] = 0,
    [0x12] = 0, [0x13] = 0, [0x14] = 0, [0x15] = 0,
    [0x16] = 0, [0x17] = 0, [0x18] = 0, [0x19] = 0,
    [0x1A] = 0, [0x1B] = 0, [0x1C] = 0, [0x1D] = 0,
    [0x1E] = 0, [0x1F] = 0, 
    [0x20] = 0, //We consider space as invalid

    // delete (0x7F)
    [0x7F] = 0,
};

typedef enum {
    REQUEST_LINE,
    HEADER_LINES,
    BODY,
} ParserState;

static const char CRLF[] = "\r\n";

static Method get_method(SpanString *method){
  if(strncmp(method->start, "GET", 3) == 0){
    return GET;
  }else if(strncmp(method->start, "POST", 4) == 0){
    return POST;
  }else if(strncmp(method->start, "PUT", 3) == 0){
    return PUT;
  }else if(strncmp(method->start, "PATCH", 5) == 0){
    return PATCH;
  }else if(strncmp(method->start, "DELETE", 6) == 0){
    return DELETE;
  }else if(strncmp(method->start, "HEAD", 4) == 0){
    return HEAD;
  }else if(strncmp(method->start, "OPTIONS", 7) == 0){
    return OPTIONS;
  }else{
    return BAD;
  }
}

static int validate_uri(SpanString *uri){
    char *end = uri->start + uri->span;
    for(char *cursor = uri->start; cursor < end; cursor++){
        if(cursor == uri->start){
            if(*cursor != '/'){
                return -1;
            }
        }else{
            if(!printable_char[*cursor]){
                return -1;
            }
        }
    }
    return 0;
}

static int get_version(SpanString *version){
  if(strncmp(version->start, "HTTP/1.0", 8) == 0){
    return V1_0;
  }else if(strncmp(version->start, "HTTP/1.1", 8) == 0){
    return V1_1;
  }else{
    return INVALID;
  }
}

static int process_request(SpanString *method, SpanString *uri, SpanString *version, RequestLine *reqLine){
    reqLine->method = get_method(method);
    if(reqLine->method == BAD){
        return -1;
    }

    if(validate_uri(uri) != 0){
        return -1;
    }
    reqLine->uri = *uri;
    
    reqLine->version = get_version(version);
    if(reqLine->version == INVALID){
        return -1;
    }     
    return 0;
}

static inline size_t min(size_t a, size_t b){
    return a < b ? a : b;
}

static void process_header(SpanString *hdrName, SpanString *hdrValue, HeaderMap *headers, size_t headersLen){
    for(int i = 0; i < headersLen; i++){
        if(strncmp(hdrName->start, headers[i].header, min(hdrName->span, strlen(headers[i].header))) == 0){
            headers[i].value = hdrValue;
        }
    }
}

int parse_http_request(SpanString request, RequestLine *reqLine, HeaderMap *headers, size_t headersLen){
    ParserState state = REQUEST_LINE;
    char *reqEnd = request.start + request.span;  
    char *methodStart = NULL,    *uriStart = NULL,    *versionStart = NULL,
         *methodEnd = NULL,      *uriEnd = NULL,      *versionEnd = NULL;
    SpanString method, uri, version;
    char *hdrNameStart = NULL,   *hdrValueStart = NULL,
         *hdrNameEnd = NULL,     *hdrValueEnd = NULL;
    SpanString hdrName, hdrValue;
    char *hdrTokenEnd = NULL;
    int onToken = 0; //state variable used to exclude trailing whitespace on header values
    
    for(char *cursor = request.start; cursor < reqEnd; cursor++){
       switch(state){
           case REQUEST_LINE:
               if(methodStart == NULL){ //Determine if first char is valid
                   if(printable_char[*cursor]){
                       methodStart = cursor;
                   }else{
                       return BAD_REQUEST;
                   }
               }else if(methodEnd == NULL){ //Keep chugging along method until whitespace found
                   if(!printable_char[*cursor]){
                       if(*cursor == ' '){
                           methodEnd = cursor;
                           method.start = methodStart;
                           method.span = methodEnd - methodStart;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(uriStart == NULL){ // Keep chugging along whitespace until uri char is found
                   if(printable_char[*cursor]){
                       uriStart = cursor; 
                   }else{
                       if(*cursor != ' '){
                           return BAD_REQUEST;
                       }
                   }
               }else if(uriEnd == NULL){ // Keep chugging along URI string until whitespace is found
                   if(!printable_char[*cursor]){
                       if(*cursor == ' '){
                           uriEnd = cursor;
                           uri.start = uriStart;
                           uri.span = uriEnd - uriStart;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(versionStart == NULL){ // Keep chugging along whitespace until first version char is found
                   if(printable_char[*cursor]){
                       versionStart = cursor; 
                   }else{
                       if(*cursor != ' '){
                           return BAD_REQUEST;
                       }
                   }
               }else if(versionEnd == NULL){ // Keep chugging along version chars until whitespace or CRLF is found
                   if(!printable_char[*cursor]){
                       if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                           versionEnd = cursor;
                           version.start = versionStart;
                           version.span = versionEnd - versionStart;
                           if(process_request(&method, &uri, &version, reqLine) == 0){
                               state = HEADER_LINES;
                               cursor++;
                           }else{
                               return BAD_REQUEST;
                           }
                       }else if(*cursor == ' '){
                           versionEnd = cursor;
                           version.start = versionStart;
                           version.span = versionEnd - versionStart;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else{ // Keep chugging through whitespace until CRLF is found
                   if(*cursor != ' '){
                       if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                           if(process_request(&method, &uri, &version, reqLine) == 0){
                               state = HEADER_LINES;
                               cursor++;
                           }else{
                               return BAD_REQUEST;
                           }
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }
               break;
           case HEADER_LINES:
               if(hdrNameStart == NULL){ //Make sure there's printable character at start
                   if(printable_char[*cursor]){
                       hdrNameStart = cursor;
                   }else if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                       cursor++;
                       state = BODY;
                   }else{
                       return BAD_REQUEST;
                   }
               }else if(hdrNameEnd == NULL){ //Keep chugging along until colon's encountered
                   if(*cursor == ':'){
                       hdrNameEnd = cursor;
                   }else if(!printable_char[*cursor]){
                       return BAD_REQUEST;
                   }
               }else if(hdrValueStart == NULL){ //Keep chugging through whitespace until character found
                   if(*cursor != ' '){
                       if(printable_char[*cursor]){
                           hdrValueStart = cursor;
                           onToken = 1;
                       }else{
                           return BAD_REQUEST;
                       }
                   }
               }else if(hdrValueEnd == NULL){ //Keep chugging along until CRLF found
                   if(*cursor == ' '){
                       if(onToken){
                           hdrTokenEnd = cursor;
                           onToken = 0;
                       }
                   }else if(printable_char[*cursor]){
                       onToken = 1;
                   }else if(*cursor == CRLF[0] && *(cursor + 1) == CRLF[1]){
                       if(onToken){
                           hdrValueEnd = cursor;
                       }else{
                           hdrValueEnd = hdrTokenEnd;
                       }
                       cursor++;
                       hdrName.start = hdrNameStart;
                       hdrName.span = hdrNameEnd - hdrNameStart;
                       hdrValue.start = hdrValueStart; 
                       hdrValue.span = hdrValueEnd - hdrValueStart;
                       process_header(&hdrName, &hdrValue, headers, headersLen);
                       hdrNameStart = hdrNameEnd = hdrValueStart = hdrValueEnd = hdrTokenEnd = NULL;
                   }else{
                       return BAD_REQUEST;
                   }
               }
               break;
           case BODY:
               break;
       }
    } 
    return 200;
}
