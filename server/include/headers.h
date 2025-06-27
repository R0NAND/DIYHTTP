#ifndef HEADERS
#define HEADERS
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
  char *accept;
  char *acceptCharset;
  char *acceptEncoding;
  char *acceptLanguage;
  char *allow;
  char *authorization;
  char *cacheControl;
  char *connection;
  char *cookie;
  char *contentEncoding;
  char *contentLanguage;
  char *contentLength;
  char *contentLocation;
  char *contentRange;
  char *contentType;
  char *date;
  char *expect;
  char *expires;
  char *from;
  char *host;
  char *ifMatch;
  char *ifModifiedSince;
  char *ifNoneMatch;
  char *ifRange;
  char *ifUnmodifiedSince;
  char *lastModified;
  char *proxyAuthorization;
  char *range;
  char *referer;
  char *te;
  char *transferEncoding;
  char *upgrade;
  char *userAgent;
  char *via;
} RequestHeaders;

typedef struct {
    const char *header;
    char **value;
} HeaderMap;
#endif
