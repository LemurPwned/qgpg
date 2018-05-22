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
  char sendline[BUF_SIZE];
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

  while(true)
  {
      bzero(sendline, BUF_SIZE);
      bzero(recvline, BUF_SIZE);
      // get stdin
      // fgets(sendline, BUF_SIZE, stdin);
      printf("%s\n", "Sending struct");
      construct_message_type(sockfd, 1);
      // writen(sockfd, sendline, strlen(sendline)+1);
      read(sockfd, recvline, BUF_SIZE);
      printf("%s\n", recvline);
  }
  return 0;
}
