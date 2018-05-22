#include <sys/types.h>   /* basic system data types */
#include <sys/socket.h>  /* basic socket definitions */
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>   /* inet(3) functions */
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>

// #include "daemon.c"
#include "sock_utils.h"

#define SPECIFIED_TIMEOUT 10
#define BUF_SIZE 1024

volatile sig_atomic_t wait_timeout = 1;

void timeout_handler (int sig){
  wait_timeout = 0;
}

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
  char msg[BUF_SIZE];

  int listen_fd, comm_fd, alice_fd, bob_fd;

  struct sockaddr_in servaddr;

	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(22000);

  int on = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
     printf("SO_REUSEADDR failed to be set");
  }

  bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  printf("Starting a server!\n");
  if (listen(listen_fd, 10) == 0) printf("Listening\n");
	else printf("Error\n");

  addr_size = sizeof serverStorage;

  alice_fd = accept(listen_fd, (struct sockaddr *) &serverStorage, &addr_size);
  if (alice_fd < 0){
    perror("Alice failed to connect");
  }
  else{
    strcpy(msg, "Alice connected. Waiting for Bob\n");
    printf(msg);
    write(alice_fd, msg, strlen(msg));
    // start signal count
  }

  struct sigaction siga;
  siga.sa_handler = timeout_handler;
  siga.sa_flags = 0;

  if (sigaction(SIGALRM, &siga, NULL) < 0){
    perror("Sigaction");
  }
  alarm(SPECIFIED_TIMEOUT);

  bob_fd = accept(listen_fd, (struct sockaddr *) &serverStorage, &addr_size);
  if (bob_fd < 0){
    perror("Bob failed to connect");
  }
  else{
    printf("Bob connected\n");
  }
  if (!wait_timeout){
    perror("Timeout exceeded");
    strcpy(msg, "Bob failed to connect in time. Closing socket...\n");
    write(alice_fd, msg, strlen(msg));
    sleep(2);
    // close(alice_fd);
    // exit(-1);
  }
  else {
    wait_timeout = 1; //reset signal
    bzero(&msg, sizeof(msg));
    strcpy(msg, "Both parties connected\n");
    write(alice_fd, msg, strlen(msg));
    write(bob_fd, msg, strlen(msg));
  }

  //TODO: Enable deamon when app is ready
  //  skeleton_daemon();

  while(true){
      bzero(client_str, BUF_SIZE);
      bzero(cmdline, BUF_SIZE);

    	read(alice_fd, client_str, BUF_SIZE);
    	printf("%s\n", client_str);
    	read(bob_fd, client_str, BUF_SIZE);
    	printf("%s\n", client_str);

      fgets(cmdline, BUF_SIZE, stdin);

    	writen(alice_fd, cmdline, strlen(cmdline));
    	writen(bob_fd, cmdline, strlen(cmdline));
	 }

  return 0;
}
