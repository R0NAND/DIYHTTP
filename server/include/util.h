#ifndef UTIL
#define UTIL
#include "span_string.h"
int isFile(const char *filepath);
char *joinPaths(SpanString *root, SpanString *child);
#endif
