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
#define MAX_SEQUENCE_EXCHANGE 3

enum message_type {
  POLARIZATION_SND,
  POLARIZATION_REQ,
  TIMEOUT_EXCEEDED,
  KEY_EXNG_INIT,
  KEY_SND
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

struct key_receive{
    char random_byte[64];
    char key[200]; // pointer to first key
    unsigned char key_hash[crypto_generichash_BYTES];
};

typedef struct key_exchange_register{
  char **key; // pointer to first key
  unsigned char *key_mask; // pointer to the first mask
} MKEY;

int construct_message_type(int, int);
int construct_server_message(int, int);
int construct_server_key_message(int, char master_key[100]);

int receive_message(int, int, struct qgpg_data *);
int receive_key_message(int, struct key_receive *);

void generate_random_byte_string(char string_buffer[64]);
void polarization_comparison(char input_buffer[64], char guess_buffer[64],
                             MKEY, int seq);
void rigid_key_extraction(MKEY, char input_buffer[64], int);

void binary_form(unsigned char*);
#endif
