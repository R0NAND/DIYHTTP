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

char *joinPaths(char *root, char *child){
  size_t len = strlen(root) + strlen(child) + 1;
  char *path = malloc(len); 
  //TODO check for null?
  snprintf(path, len, "%s%s", root, child);
  return path;
}
