#ifndef PARSING
#define PARSING

#include <stddef.h>

#include "headers.h"
#include "http_enums.h"

typedef struct {
    Methods method;
    char *uri;
    Versions version;
} RequestLine;
    
int parse_http_request(char *request, size_t reqLen, RequestLine *reqLine, HeaderMap *headers, size_t headersLen);

#endif

