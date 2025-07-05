#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT "3490"
#define BACKLOG 10
#define MAX_DATA_SIZE 100
int main(){
  struct addrinfo hints, *res;
  int sockfd;
  char *request = "GET / HTTP/1.1\r\n"
                  "Host: RoDawg";
  int len, bytes_sent;
  len = strlen(request);

  char buf[MAX_DATA_SIZE];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo("127.0.0.1", SERVER_PORT, &hints, &res);

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  
  connect(sockfd, res->ai_addr, res->ai_addrlen);
  while(1){
    printf("sending bytes\n");
    bytes_sent = send(sockfd, request, len, 0);
    bytes_sent = recv(sockfd, buf, MAX_DATA_SIZE - 1, 0);
    printf("received %s", buf); 
  }
  return 0;  
}

