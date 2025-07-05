#ifndef HEADERS
#define HEADERS

#include "span_string.h"
extern const char ACCEPT[];
extern const char ACCEPT_CHARSET[];
extern const char ACCEPT_ENCODING[];
extern const char ACCEPT_LANGUAGE[];
extern const char ALLOW[];
extern const char AUTHORIZATION[];
extern const char CACHE_CONTROL[];
extern const char CONNECTION[];
extern const char COOKIE[];
extern const char CONTENT_ENCODING[];
extern const char CONTENT_LANGUAGE[];
extern const char CONTENT_LENGTH[];
extern const char CONTENT_LOCATION[];
extern const char CONTENT_RANGE[];
extern const char CONTENT_TYPE[];
extern const char DATE[];
extern const char EXPECT[];
extern const char EXPIRES[];
extern const char FROM[];
extern const char HOST[];
extern const char IF_MATCH[];
extern const char IF_MODIFIED_SINCE[];
extern const char IF_NONE_MATCH[];
extern const char IF_RANGE[];
extern const char IF_UNMODIFIED_SINCE[];
extern const char LAST_MODIFIED[];
extern const char PROXY_AUTHORIZATION[];
extern const char RANGE[];
extern const char REFERER[];
extern const char TE[];
extern const char TRANSFER_ENCODING[];
extern const char UPGRADE[];
extern const char USER_AGENT[];
extern const char VIA[];

typedef struct {
  SpanString accept;
  SpanString acceptCharset;
  SpanString acceptEncoding;
  SpanString acceptLanguage;
  SpanString allow;
  SpanString authorization;
  SpanString cacheControl;
  SpanString connection;
  SpanString cookie;
  SpanString contentEncoding;
  SpanString contentLanguage;
  SpanString contentLength;
  SpanString contentLocation;
  SpanString contentRange;
  SpanString contentType;
  SpanString date;
  SpanString expect;
  SpanString expires;
  SpanString from;
  SpanString host;
  SpanString ifMatch;
  SpanString ifModifiedSince;
  SpanString ifNoneMatch;
  SpanString ifRange;
  SpanString ifUnmodifiedSince;
  SpanString lastModified;
  SpanString proxyAuthorization;
  SpanString range;
  SpanString referer;
  SpanString te;
  SpanString transferEncoding;
  SpanString upgrade;
  SpanString userAgent;
  SpanString via;
} RequestHeaders;

typedef struct {
    const char *header;
    SpanString *value;
} HeaderMap;
#endif
