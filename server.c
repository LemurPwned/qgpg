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
#include "qgpg_proto_def.h"
#include "qgpg_proto_def.c"

#define SPECIFIED_TIMEOUT 10
#define BUF_SIZE 1024
#define MAX_SEQUENCE_EXCHANGE 3
volatile sig_atomic_t wait_timeout = 1;
struct sigaction siga;

void timeout_handler (int sig){
  wait_timeout = 0;
}

void reset_and_start_timer(int time_required){
  wait_timeout = 1; // resets the signal timer

  // sets up the alarm again
  siga.sa_handler = timeout_handler;
  siga.sa_flags = 0; 
  alarm(time_required);
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

  struct qgpg_data data;
  // zero the data
  bzero(&data, sizeof(struct qgpg_data));
  struct key_exchange_register master_key;
  // zero the master key
  bzero(&master_key, sizeof(struct key_exchange_register));

  int sequence_number = 1;
  int msg_type, saved_timeout_state;
  wait_timeout = 1;
  saved_timeout_state = 1;
  // #define SPECIFIED_TIMEOUT 2
  // initialize by sending to alice
  construct_server_message(alice_fd, POLARIZATION_REQ);
  while(true){
      // receive from alice
      printf("%s\n", "Waiting for Alice...");
      reset_and_start_timer(SPECIFIED_TIMEOUT/5);
      msg_type = receive_message(alice_fd, sequence_number, &data);
      saved_timeout_state = wait_timeout;
      alarm(0); // cancel alarm 
      if (msg_type == POLARIZATION_SND){
        // actual message, check the data
        printf("%s\n", "Polarization received");
        printf("%s\n", data.polarization_basis);
        bzero(&data, sizeof(struct qgpg_data));
        // can send request
        // write to bob that he should respond
        if (saved_timeout_state){
          construct_server_message(bob_fd, POLARIZATION_REQ);
        }
        else{
          printf("EXCEEDED TIME\n");
          construct_server_message(bob_fd, TIMEOUT_EXCEEDED);
        }
      }
      else {
        printf("%s\n", "Quitting, invalid sequence");
        close(alice_fd);
        close(bob_fd);
        exit(-1);
      }
      sleep(SPECIFIED_TIMEOUT/5);
      printf("%s\n", "Receiving from Bob...");
      reset_and_start_timer(SPECIFIED_TIMEOUT/5);
      msg_type = receive_message(bob_fd, sequence_number, &data);
      saved_timeout_state = wait_timeout;
      alarm(0);
      if (msg_type == POLARIZATION_SND){
        printf("%s\n", "Polarization received");
        printf("%s\n", data.polarization_basis);
        bzero(&data, sizeof(struct qgpg_data));
        // can send request
        // write to alice that she should send something
        if (saved_timeout_state){
          construct_server_message(alice_fd, POLARIZATION_REQ);
        }
        else{
          printf("EXCEEDED TIME\n");
          construct_server_message(alice_fd, TIMEOUT_EXCEEDED);
        }      
      }
      else {
        printf("%s\n", "Quitting, invalid sequence");
        close(alice_fd);
        close(bob_fd);
        exit(-1);
      }
      if (sequence_number >= MAX_SEQUENCE_EXCHANGE){
        // polarization exchange ended
        // time to exchange keys
        printf("%s\n", "Communication has finished");
        break;
      }
      sequence_number++; // await next message
	 }

  return 0;
}
