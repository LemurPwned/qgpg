#include <sys/types.h>   /* basic system data types */
#include <sys/socket.h>  /* basic socket definitions */
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>   /* inet(3) functions */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>                /* timespec{} for pselect() */

#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>




#include "sock_utils.h"
#define BUF_SIZE 1000

int command_processor(char *command){
  if (strcmp(command, "quit") == 0){
    exit(0);
  }
  else if (strcmp(command, "show") == 0){
    return 1;
  }
  else {
    return 0;
  }
}


int main(){
  char client_str[BUF_SIZE];
  char cmdline[BUF_SIZE];

  int listen_fd, comm_fd;

  struct sockaddr_in servaddr;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(22000);


  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

  printf("Starting a server!\n");
  listen(listen_fd, 10);

  if ((comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) < 0){
      printf("Failed connection");
      exit(0);
  }
  else printf("System connected\n");

  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
      printf("SO_REUSEADDR failed to be set");
  }

  char msg[BUF_SIZE];

  while(true){
      bzero(client_str, BUF_SIZE);
      bzero(cmdline, BUF_SIZE);

      read(comm_fd, client_str, BUF_SIZE);
      printf("%s\n", client_str);

      // remove CR CF command
      // remove_char_from_string('\n', client_str);
      // process client command

      fgets(cmdline, BUF_SIZE, stdin);
      // remove CR CF command
      // remove_char_from_string('\n', cmdline);

      writen(comm_fd, cmdline, strlen(cmdline));
    }
  return 0;
}
