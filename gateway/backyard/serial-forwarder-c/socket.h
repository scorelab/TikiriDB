#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/socket.h>
#include <stdint.h>

typedef int SOCKET;

/*
 * Unified socket address. For IPv6 support, add IPv6 address structure
 * in the union u.
 */
struct usa {
  socklen_t len;
  struct sockaddr_storage sa;
};

/*
 * Structure used to describe listening socket, or socket which was
 * accept()-ed by listening thread.
 */
struct socket {
  SOCKET sockfd;    /* Listening socket */
  struct usa usa; /* Socket address */
};

/*
 * Get sockaddr, IPv4 or IPv6:
 */
void * get_in_addr(struct sockaddr *sa);

/*
 * Write data to the socket. Return number of bytes written.
 */
uint64_t send_all(SOCKET sock, const char *buf, uint64_t len);

int send_chunk(SOCKET sock, const char *buf, unsigned int len);

int send_as_chunks(SOCKET sock, const char *buf, unsigned int len);

int sock_printf(SOCKET sockfd, const char *fmt, ...);

int create_server_socket(int port, struct socket * s_socket, int queue_len);

unsigned int get_ready_bytes(SOCKET socketfd);

int socket_wait(SOCKET socketfd, int mask, unsigned int m_secs);

#endif /* __SOCKET_H__ */



