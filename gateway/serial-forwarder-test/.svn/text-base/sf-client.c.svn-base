
#include "socket.h"
#include "sf-client.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>

//#define DEBUG 1

#ifdef DEBUG
#define LOG_DEBUG(...) printf("[ DEBUG ] "__VA_ARGS__)
#define LOG_ERROR(...) printf("[ ERROR ] "__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#endif 


/*---------------------------------------------------------------------------*/
int
handshake(int sockfd, int mode)
{
  char buf[4]; // for initial handshaking...
  int k;

  // set the version of the sf client
  buf[0] = VERSION_MAJOR;
  buf[1] = VERSION_MINOR;
  // set connecting mode
  buf[2] = mode;
  buf[3] = 0; // not used for now.

  k = send_all(sockfd, buf, 4);
  if(k != 4) {
    perror("Can not send handshaking bytes to sf server");
    return ERR_UNKNOWN;
  }

  k = read_all(sockfd, buf, 4);
  if(k != 4) {
    perror("Can not receive handshaking bytes fron sf server");
    return ERR_UNKNOWN;
  }

  return buf[2];
 
}

/*---------------------------------------------------------------------------*/
int send_packet(int sockfd, packet_t * packet)
{
  char buf[4];
  int k;
  
  buf[0] = packet->len & 0xFF;
  buf[1] = (packet->len >> 8) & 0xFF;
  buf[2] = packet->type;
  buf[3] = 0; // not used for now

  k = send_all(sockfd, buf, 4);
  if(k != 4) {
    perror("Can not send packet meta data");
    return -1;
  }

  k = send_all(sockfd, packet->data, packet->len);
  if(k != packet->len) {
    perror("Can not send packet data");
    return -1;
  }
  
  return 0;
}
/*---------------------------------------------------------------------------*/
int receive_packet(int sockfd, packet_t * packet)
{
  char buf[4];
  int len;
  int k;
  
  LOG_DEBUG("reading packet...\n");
  if(packet == NULL) {
    return -1;
  }
  
  k = read_all(sockfd, buf, 4);
  if(k != 4) {
    perror("Can not receive packet meta data");
    return -1;
  }
  len = buf[0] + (buf[1] << 8);
  LOG_DEBUG("packet size : %d\n", len);
  if(len > MAX_PKT_SIZE) {
    printf("maximum packet size exceeded. len %d\n", len);
    return -1;
  }

  k = read_all(sockfd, packet->data, (uint64_t)len);
  if(k != len) {
    perror("Can not receive packet data");
    return -1;
  }
  packet->type = buf[2];
  packet->len = len;
  LOG_DEBUG("packet read done\n");
  return 0;
}
static int
get_random(int max) {
  return rand() % max;
}
/*---------------------------------------------------------------------------*/
void generate_dummy_packet(packet_t * packet)
{
  srand(time(NULL));
  int len = get_random(MAX_PKT_SIZE);
  int i;

  if(packet == NULL) {
    return;
  }
  for(i=0; i < len; i++) {
    packet->data[i] = get_random(254);
  }
  packet->len = len;

} 
/*---------------------------------------------------------------------------*/
void 
print_packet(packet_t * packet)
{
#define ROW_COUNT 16

  int i, k, printed, left, end;
  int nrows;

  if(packet == NULL) {
    return;
  }
  
  for(i=0; i< 2 * ROW_COUNT; i++) {
    printf("=");
  }  
  printf("\n");
  printf("length : %d\n", packet->len);
  printf("data   :\n");
  nrows = packet->len / ROW_COUNT;
  for(i = 0; i <= nrows; i++) {
    printf("------- ");
    printed = ROW_COUNT * i;
    left = packet->len - printed;
    end = printed + ((left > ROW_COUNT) ? ROW_COUNT : left);
    for(k = printed; k < end; k++) {
      printf("%02X ", (unsigned char)packet->data[k]);
    }
    printf("\n");
  }
  fflush(stdout);
}
/*---------------------------------------------------------------------------*/









