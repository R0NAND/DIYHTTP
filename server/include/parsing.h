#ifndef PARSING
#define PARSING

#include <stddef.h>

#include "headers.h"
#include "http_enums.h"
#include "span_string.h"

typedef struct {
    Method method;
    SpanString uri;
    Version version;
} RequestLine;
    
int parse_http_request(SpanString request, RequestLine *reqLine, HeaderMap *headers, size_t headersLen);

#endif

