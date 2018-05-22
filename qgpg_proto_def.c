#include "qgpg_proto_def.h"
#include "sock_utils.h"

int construct_message_type(int socketfd, int seq){
  struct qgpg_message to_send;
  struct qgpg_data polarization_data;

  time_t current_time = time(NULL);
  to_send.type = POLARIZATION_SND;
  to_send.date_sent = *ctime(&current_time);
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
  return 1;
}

int receive_message(int socketfd, int seq){
  struct qgpg_message to_receive;

  if(read(socketfd, (char *)&to_receive, sizeof(to_receive)) < 0){
    perror("Failed to read from the socket");
  }
  printf("%d\n", to_receive.type);
  switch (to_receive.type) {
    case POLARIZATION_SND:
      printf("%s\n", "Polarization received");
      struct qgpg_data *recv_data = (struct qgpg_data*)&to_receive.payload;
      printf("%s\n", recv_data->polarization_basis);
      break;

    case POLARIZATION_RCV:
      break;

    case TIMEOUT_EXCEEDED:
      break;

    default:
      break;
  }

  return 0;
}

void generate_random_byte_string(char string_buffer[64]){
  if (sodium_init() < 0) {
    printf("%s\n", "Failed to initialize library");
      /* panic! the library couldn't be initialized, it is not safe to use */
  }
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
