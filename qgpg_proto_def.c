#include "qgpg_proto_def.h"
#include "sock_utils.h"


int construct_server_message(int socketfd){
  struct qgpg_message server_message;
  struct qgpg_data null_data;
  // data is empty
  bzero(&null_data, sizeof(null_data));

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  server_message.date_sent =  tm.tm_sec;
  server_message.type = POLARIZATION_SND; // polarization send request

  // copy zeros to message payload
  memcpy((void *) server_message.payload, (void*)&null_data,
                                    sizeof(null_data));
  // write message to the server
  writen(socketfd, (char*)&server_message, sizeof(server_message));
  return 0;
}

int construct_message_type(int socketfd, int seq){
  struct qgpg_message to_send;
  struct qgpg_data polarization_data;

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  to_send.type = POLARIZATION_RCV; // receive polarization
  to_send.date_sent = tm.tm_sec;
  // intialize random state
  char string_buffer[64];
  polarization_data.data_id = seq;
  generate_random_byte_string(string_buffer);
  strcpy(polarization_data.polarization_basis, string_buffer);

  generate_random_byte_string(string_buffer);
  strcpy(polarization_data.polarization_orthogonality, string_buffer);

  printf("%s\n", polarization_data.polarization_basis);
  printf("%s\n", polarization_data.polarization_orthogonality);

  memcpy((void *) to_send.payload, (void*)&polarization_data,
                                    sizeof(polarization_data));

  writen(socketfd, (char*)&to_send, sizeof(to_send));
  return 0;
}

int receive_message(int socketfd, int seq){
  struct qgpg_message to_receive;

  if(read(socketfd, (char *)&to_receive, sizeof(to_receive)) < 0){
    perror("Failed to read from the socket");
  }
  printf("%d\n", to_receive.type);
  printf("Date received: %d\n", to_receive.date_sent);
  switch (to_receive.type) {
    case POLARIZATION_SND:
      printf("%s\n", "Polarization received");
      struct qgpg_data *recv_data = (struct qgpg_data*)&to_receive.payload;
      printf("%s\n", recv_data->polarization_basis);
      return POLARIZATION_SND;
      break;

    case POLARIZATION_RCV:
      printf("%s\n", "Polarization send request received");
      return POLARIZATION_RCV;
      break;

    case TIMEOUT_EXCEEDED:
      printf("%s\n", "Connection timeout has been exceeded");
      break;

    default:
      break;
  }

  return 0;
}

void generate_random_byte_string(char string_buffer[64]){
  #ifndef SODIUM_INITIALIZE
  #define SODIUM_INITIALIZE
    if (sodium_init() < 0) {
      printf("%s\n", "Failed to initialize library");
        /* panic! the library couldn't be initialized, 
        it is not safe to use */
    }
  #endif

  char snum[5];
  uint32_t rand_int;
  bzero(string_buffer, sizeof(&string_buffer));
  for (int i = 0; i < 8; i++){
    rand_int = randombytes_uniform(10)%2;
    sprintf(snum, "%d", rand_int);
    strcat(string_buffer, snum);
    bzero(&snum, sizeof(snum));
  }
  printf("Generated string is: %s\n", string_buffer);
}

void pol_comparison(char input_buffer[64], char guess_buffer[64],
                    char secret_buffer[64]){
  // unsigned char because we send just one byte via channel
  unsigned char input = strtol(input_buffer, NULL, 2); 
  //take binary of both polarizations
  unsigned char guess = strtol(guess_buffer, NULL, 2);
  unsigned char mask = ~(input^guess); 
  //mask should now contain only ones that do agree
}