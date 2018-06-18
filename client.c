#include <sys/types.h>   /* basic system data types */
#include <sys/socket.h>  /* basic socket definitions */
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>   /* inet(3) functions */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>    /* timeval{} for select() */
#include <time.h>                /* timespec{} for pselect() */
#include <errno.h>
#include <fcntl.h>               /* for nonblocking */
#include <netdb.h>

#include "sock_utils.h"
#include "sock_utils.c"
#include "qgpg_proto_def.h"
#include "qgpg_proto_def.c"

#define BUF_SIZE 1000

ssize_t	writen(int fd, const void *vptr, size_t n);

int main(){
  int sockfd;
  char recvline[BUF_SIZE];
  struct sockaddr_in servaddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof servaddr);

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(22000);

  inet_pton(AF_INET,"127.0.0.1", &(servaddr.sin_addr));

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
    printf("%s\n", "Failed to connect");
  }
  else{
    printf("%s\n", "Connection established...\n");
  }
  int on = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
      printf("SO_REUSEADDR failed to be set");
  }
  do {
    bzero(recvline, BUF_SIZE);
    read(sockfd, recvline, BUF_SIZE);
    printf("%s\n", recvline);
  } while(strcmp(recvline, "Both parties connected\n") != 0);

  printf("Communication begins...\n");

  int sequence_number = 1;
  int msg_type;
  while(true)
  {
      // now wait for server to allow for connection
      msg_type = receive_message(sockfd, sequence_number);
      // send polarization if requested
      if (msg_type == POLARIZATION_REQ){
        printf("%s\n", "Sending response struct to server");
        construct_message_type(sockfd, sequence_number);
      }
      else if (msg_type == TIMEOUT_EXCEEDED){
        // graceful shutdown
        // close(sockfd);
        perror("Timeout error\n");
      }
      else if (msg_type == POLARIZATION_SND){
        perror("REQUEST HAS BEEN SENT\n");
      }
      else {
        perror("Unknown server request");
        printf("UNKOWN TYPE %d\n", msg_type);
        }
      sequence_number++;
  }
  return 0;
}
