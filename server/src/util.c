#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"

int isFile(const char *filepath){
  struct stat st;
  printf("%s\n", filepath);
  return (stat(filepath, &st) == 0);
}

char *joinPaths(SpanString *root, SpanString *child){
  size_t len = root->span + child->span + 1;
  char *path = malloc(len);
  if(path == NULL){
      fprintf(stderr, "Memory allocation failed\n");
      exit(1);
  }
  snprintf(path, len, "%.*s%.*s", (int)root->span, root->start, (int)child->span, child->start);
  return path;
}
