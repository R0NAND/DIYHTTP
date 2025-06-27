#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define MYPORT "3490"
#define BACKLOG 10
#define MAX_DATA_SIZE 100
int main(){
  printf("start\n");
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
  char buf[MAX_DATA_SIZE]; 
  int msg_len, msg_bytes_rcvd;

  msg_len = strlen(buf);

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, MYPORT, &hints, &res);
  
  printf("starting socket\n");
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  printf("binding sockeft\n");  
  bind(sockfd, res->ai_addr, res->ai_addrlen); 
  printf("listening\n"); 
  listen(sockfd, BACKLOG);
  addr_size = sizeof their_addr;
  while(1){
    printf("acceptin\n");
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size); 
    printf("accepted\n");
    if(!fork()){
      close(sockfd);
      
      msg_bytes_rcvd = recv(new_fd, buf, MAX_DATA_SIZE - 1, 0);
      if(msg_bytes_rcvd > 0){
        printf("%s\n", buf);
	const char *response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n"
                "\r\n";
	msg_bytes_rcvd = send(new_fd, response, strlen(response), 0);
	printf("msg bytes int: %i\n", msg_bytes_rcvd);
      }
      close(new_fd);
      exit(0);
    }
  }
  printf("sent bytes! Shutting down\n");
  printf("%s\n", buf);
  return 0;  
}
