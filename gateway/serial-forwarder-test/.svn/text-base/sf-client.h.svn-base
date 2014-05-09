#ifndef __SF_CLIENT_H__
#define __SF_CLIENT_H__

#include "socket.h"

#ifdef CONF_MAX_PKT_SIZE
#define MAX_PKT_SIZE CONF_MAX_PKT_SIZE
#else
#define MAX_PKT_SIZE 200
#endif

#define PKT_TYPE_DATA 1
#define PKT_TYPE_DEBUG 2


typedef struct packet {
  char data[MAX_PKT_SIZE];
  int len;
  int type;
} packet_t;


/* client modes */

#define CLIENT_MODE_R  1 // read
#define CLIENT_MODE_W  2 // write
#define CLIENT_MODE_RW 3 // read + write

/* hanshaking error codes  */
#define ERR_UNKNOWN -1
#define ERR_OK 1
#define ERR_INVALID_MODE 2
#define ERR_UNSUPPORTED_VERSION 3
#define ERR_READ_CLIENTS_FULL 4
#define ERR_WRITE_CLIENTS_FULL 5


int handshake(int sockfd, int mode);

int send_packet(int sockfd, packet_t * packet);

int receive_packet(int sockfd, packet_t * packet);

void generate_dummy_packet(packet_t * packet);

void print_packet(packet_t * packet);

#endif /* __SF_CLIENT_H__ */
