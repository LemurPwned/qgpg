#ifndef QGPG_PROTO_DEF
#define QGPG_PROTO_DEF

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sodium.h>
#include <string.h>
#include <unistd.h>


#define MAX_PAYLOAD_SIZE 500
#define BUFF_SIZE 1000

enum message_type {
  POLARIZATION_SND,
  POLARIZATION_REQ,
  TIMEOUT_EXCEEDED,
};

struct qgpg_data{
  int data_id;
  char polarization_basis[64];
  char polarization_orthogonality[64];
};

struct qgpg_message{
  enum message_type type;
  int date_sent;
  char payload[MAX_PAYLOAD_SIZE];
};

struct key_exchange_register{
  unsigned char **key; // pointer to first key
  unsigned char **key_mask; // pointer to the first mast
};

int construct_message_type(int, int);
int construct_server_message(int, int);

int receive_message(int, int, struct qgpg_data *);
void generate_random_byte_string(char string_buffer[64]);
void pol_comparison(char input_buffer[64], char guess_buffer[64],
                    struct key_exchange_register*, int seq);

void binary_form(unsigned char*);
#endif
