#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "parsing.h"
#include "headers.h"
#include "http_enums.h"
#include "util.h"

int main(){
  int responseCode = 0;
  char request[] = "GET index.html HTTP1.1\r\n"
	           "User-Agent: cURL\r\n"
	           "Connection: Keep-Alive\r\n\r\n";
  size_t reqLen = strlen(request);
  char response[1024];

  RequestHeaders requestHeaders;
  memset(&requestHeaders, 0, sizeof(requestHeaders));
  HeaderMap headerMap[] = {
    { ACCEPT,                   &requestHeaders.accept },
    { ACCEPT_CHARSET,           &requestHeaders.acceptCharset },
    { ACCEPT_ENCODING,          &requestHeaders.acceptEncoding },
    { ACCEPT_LANGUAGE,          &requestHeaders.acceptLanguage },
    { ALLOW,                    &requestHeaders.allow },
    { AUTHORIZATION,            &requestHeaders.authorization },
    { CACHE_CONTROL,            &requestHeaders.cacheControl },
    { CONNECTION,               &requestHeaders.connection },
    { COOKIE,                   &requestHeaders.cookie },
    { CONTENT_ENCODING,         &requestHeaders.contentEncoding },
    { CONTENT_LANGUAGE,         &requestHeaders.contentLanguage },
    { CONTENT_LENGTH,           &requestHeaders.contentLength },
    { CONTENT_LOCATION,         &requestHeaders.contentLocation },
    { CONTENT_RANGE,            &requestHeaders.contentRange },
    { CONTENT_TYPE,             &requestHeaders.contentType },
    { DATE,                     &requestHeaders.date },
    { EXPECT,                   &requestHeaders.expect },
    { EXPIRES,                  &requestHeaders.expires },
    { FROM,                     &requestHeaders.from },
    { HOST,                     &requestHeaders.host },
    { IF_MATCH,                 &requestHeaders.ifMatch },
    { IF_MODIFIED_SINCE,        &requestHeaders.ifModifiedSince },
    { IF_NONE_MATCH,            &requestHeaders.ifNoneMatch },
    { IF_RANGE,                 &requestHeaders.ifRange },
    { IF_UNMODIFIED_SINCE,      &requestHeaders.ifUnmodifiedSince },
    { LAST_MODIFIED,            &requestHeaders.lastModified },
    { PROXY_AUTHORIZATION,      &requestHeaders.proxyAuthorization },
    { RANGE,                    &requestHeaders.range },
    { REFERER,                  &requestHeaders.referer },
    { TE,                       &requestHeaders.te },
    { TRANSFER_ENCODING,        &requestHeaders.transferEncoding },
    { UPGRADE,                  &requestHeaders.upgrade },
    { USER_AGENT,               &requestHeaders.userAgent },
    { VIA,                      &requestHeaders.via },
};

  RequestLine reqLine;

  char *rootDir = "/home/ronan/Dev/DIYHTTP/server/resources";

  responseCode = parse_http_request(request, reqLen, &reqLine, headerMap, sizeof(headerMap));
  //char *resourcePath = joinPaths(rootDir, reqLine.uri);
  //if(!isFile(resourcePath)){
    //return NOT_FOUND;
  //}
  printf("Server responded with code %i", responseCode);
  return 0;
}
