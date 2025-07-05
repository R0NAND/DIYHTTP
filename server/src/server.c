#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

int main(){
  static const unsigned short int MAIN_PORT = 3490;
  static const int BACKLOG = 10;
  static const int MAX_BYTES = 1000; 
  static const int MAX_EVENTS = 10;
 
  struct sockaddr_in receiver_addr, client_addr;
  socklen_t addrlen;
  memset(&receiver_addr, 0, sizeof(receiver_addr));
  receiver_addr.sin_family = AF_INET;
  receiver_addr.sin_port = htons(MAIN_PORT);
  inet_pton(AF_INET, "127.0.0.1", &(receiver_addr.sin_addr));
  
  int receiver_fd = socket(AF_INET, SOCK_STREAM, 0);
  addrlen = sizeof receiver_addr;
  if(bind(receiver_fd, (struct sockaddr*)&receiver_addr, addrlen) < 0){
      close(receiver_fd);
      perror("server: bind");
      return 1;
  }
  if(listen(receiver_fd, BACKLOG) < 0){
      close(receiver_fd);
      perror("server: listen");
      return 1;
  }
 
  struct epoll_event ev, events[MAX_EVENTS];
  int poll_fd = epoll_create1(0);
  ev.events = EPOLLIN;
  ev.data.fd = receiver_fd;
  epoll_ctl(poll_fd, EPOLL_CTL_ADD, receiver_fd, &ev); 
  for(;;){
      int num_fd = epoll_wait(poll_fd, events, MAX_EVENTS, -1);
      for(int i = 0; i < num_fd; i++){
          if(events[i].data.fd == receiver_fd){ //TODO: Investigate whether I'd need to iterate until it returns zero...
              int new_fd = accept(receiver_fd, (struct sockaddr*)&client_addr, &addrlen);
              ev.events = EPOLLIN;
              ev.data.fd = new_fd;
              epoll_ctl(poll_fd, EPOLL_CTL_ADD, new_fd, &ev);
              printf("accepted a new connection\n");
          }else{
              printf("Got a request\n");
              char buf[MAX_BYTES];
              ssize_t n_bytes = recv(events[i].data.fd, buf, MAX_BYTES, 0);
              if(n_bytes != 0){
                  send(events[i].data.fd, "Warm regards", 12, 0);
              }else{
                  if(close(events[i].data.fd) != 0){
                      perror("server close error:");
                  }
              }
          }
      }
  }
  return 0;  
}
