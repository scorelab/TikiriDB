#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_PORT_STR_SIZE 5
#define CHUNK_SIZE 512

#ifdef CONF_MAX_REQUEST_SIZE
#define CONF_MAX_REQUEST_SIZE MAX_REQUEST_SIZE
#else
#define MAX_REQUEST_SIZE 1024
#endif



#ifdef CONF_MAX_ERROR_STR_SIZE
#define CONF_MAX_ERROR_STR_SIZE MAX_ERROR_STR_SIZE
#else
#define MAX_ERROR_STR_SIZE 512
#endif

#ifdef DEBUG
#define LOG_DEBUG(...) printf("[ DEBUG ]"__VA_ARGS__)
#define LOG_ERROR(...) printf("[ ERROR ]"__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#endif  

/*---------------------------------------------------------------------------*/
/*
 * Get sockaddr, IPv4 or IPv6:
 */
void *
get_in_addr(struct sockaddr *sa)
{
  if(sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/*---------------------------------------------------------------------------*/
int
get_port(struct sockaddr_storage *ss) {
  switch(ss->ss_family) {
    case AF_INET:
      return ((struct sockaddr_in*)ss)->sin_port;
    case AF_INET6:
      return ((struct sockaddr_in6*)ss)->sin6_port;
  }
}
/*---------------------------------------------------------------------------*/
int
get_ip_address_string(struct sockaddr_storage *ss, char *buf, size_t buflen)
{
  const char *retval;
  switch(ss->ss_family) {
    case AF_INET:
      retval = inet_ntop(ss->ss_family,
               &((struct sockaddr_in *)ss)->sin_addr, buf, buflen);
      break;

    case AF_INET6:
      retval = inet_ntop(ss->ss_family,
                &((struct sockaddr_in6 *)ss)->sin6_addr, buf, buflen);
      break;
  }

  if(retval == NULL) {
    return -1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * Write data to the socket. Return number of bytes written.
 */
uint64_t
send_all(SOCKET sock, const char *buf, uint64_t len)
{
  uint64_t sent;
  int n, k;

  sent = 0;
  while(sent < len) {

    /* How many bytes we send in this iteration */
    k = ((uint32_t)(len - sent) > INT_MAX) ? INT_MAX : (int)(len - sent);
    n = send(sock, buf + sent, k, 0);

    if(n < 0){
      break;
    }    sent += n;
  }
  return sent;
}
/*---------------------------------------------------------------------------*/
/*
 * Read data from the socket until 'len' bytes read.
 */
uint64_t
read_all(SOCKET sock, char *buf, uint64_t len)
{
  uint64_t read;
  int n;
  unsigned int k;

  read = 0;
  while(read < len) {
    
    /* How many bytes we read in this iteration */
    k = ((uint32_t)(len - read) > INT_MAX) ? INT_MAX : (int)(len - read);
    //LOG_DEBUG("k : %lu", k);
    n = recv(sock, buf + read, k, 0);
    if(n <= 0){
      break;
    }    read += n;
  }

  return read;
}
/*---------------------------------------------------------------------------*/
/*
 * Send chunked data over a socket.
 */
int
send_chunk(SOCKET sock, const char *buf, unsigned int len)
{
  char tmp[64];
  unsigned int l;

  l = sprintf(tmp, "%x\r\n", len);

  if(send_all(sock, tmp, l) != l) {
    return -1;
  }  
  if(send_all(sock, buf, len) != len) {
    return -1;   
  }
  if(send_all(sock, "\r\n", 2) != 2) {
    return -1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * Send data as chunks.
 */

int 
send_as_chunks(SOCKET sock, const char *buf, unsigned int len)
{
  unsigned int sent = 0;
  unsigned int n, r;

  while(sent < len) {
    n = (len - sent) / CHUNK_SIZE;
    if(n > 0) {
      if(send_chunk(sock, buf + sent, CHUNK_SIZE) < 0) {
        return -1;
      }
      sent += CHUNK_SIZE;
    } else {
      r = (len - sent) % CHUNK_SIZE;
      if(send_chunk(sock, buf + sent, r) < 0) {
        return -1;
      }
      sent += r;
    }
    
  }
  if(send_chunk(sock, "", 0) < 0){
    return -1;
  }
  return sent;
}
/*---------------------------------------------------------------------------*/
/*
 * printf function implementation for a socket
 */
int
sock_printf(SOCKET sockfd, const char *fmt, ...)
{
  int len;
  char buf[MAX_REQUEST_SIZE + 1];
  va_list args;

  va_start(args, fmt);
  len = vsnprintf(buf, MAX_REQUEST_SIZE, fmt, args);
  va_end(args);
 
  return send_all(sockfd, buf, len);;
}
/*---------------------------------------------------------------------------*/
/*
 * Create a listening socket.
 */
int 
create_server_socket(int port, struct socket * s_socket, int queue_len)
{
  int sockfd; // listen on sock_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage *sever_address;
  char port_str[MAX_PORT_STR_SIZE + 1];
  char error_str[MAX_ERROR_STR_SIZE +1];
  char s[INET6_ADDRSTRLEN];
  int yes=1;
  int rv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  sprintf(port_str, "%d", port);

  if((rv = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
    LOG_ERROR("getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_ERROR("Can not create a socket : %s", error_str);
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_ERROR("Can not set socket options : %s", error_str);
      return -1;
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_ERROR("Can not bind the socket : %s", error_str);
      continue;
    } 
    
    if(listen(sockfd, queue_len) < 0) {
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_ERROR("Can not listen to the socket : %s", error_str);
      return -1;
    }
    break;
  }

  if(p == NULL)  {
    LOG_ERROR("Could not find a socket to bind.");
    return -1;
  }

  s_socket->sockfd = sockfd;
  s_socket->usa.len = p->ai_addrlen;
  memcpy(&s_socket->usa.sa, p->ai_addr, s_socket->usa.len);

  freeaddrinfo(servinfo); 

  return 0;
}
/*---------------------------------------------------------------------------*/
int
connect_to_server(char *address, int port)
{
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  char port_str[MAX_PORT_STR_SIZE + 1];
  char error_str[MAX_ERROR_STR_SIZE +1];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  sprintf(port_str, "%d", port);

  if((rv = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
    LOG_DEBUG("getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }
  
  /* loop through all the results and connect to the first we can */
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_DEBUG("Can not create a socket : %s", error_str);
      continue;
    }

    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      strerror_r(errno, error_str, MAX_ERROR_STR_SIZE);
      LOG_DEBUG("Can not connect : %s", error_str);
      continue;
    }

    break;
  }

  if(p == NULL)  {
    LOG_DEBUG("Could not connect to %s:%d", address, port);
    return -1;
  }

  freeaddrinfo(servinfo); // all done with this structure

  return sockfd;
}
/*---------------------------------------------------------------------------*/
unsigned int
get_ready_bytes(SOCKET socketfd)
{
  unsigned int x;
  if(ioctl(socketfd, FIONREAD, &x) != 0) {
    x = 0;
  }

  return x;
}
/*---------------------------------------------------------------------------*/
/*
 * 1 - read, 2 - write, 3 - read+write
 */
int 
socket_wait(SOCKET socketfd, int mask, unsigned int m_secs)
{
  fd_set rfds, wfds;
  struct timeval tv;
  int retval;

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  if(socketfd < 0) {
    return 0;
  }

  if((mask & 1) == 1) {
    FD_SET(socketfd, &rfds);
  }

  if((mask & 2) == 2) {
    FD_SET(socketfd, &wfds);
  }

  tv.tv_sec = m_secs / 1000;
  tv.tv_usec = m_secs % 1000;

  do {
    retval =  select(socketfd + 1, &rfds, &wfds,NULL,&tv);
    switch(retval) { 	
      case 0:  /* time out */
        return 0;

      case (-1):  /* socket error */
        if (errno == EINTR) {
          break;
        }
        return 0;

      default:
        if(FD_ISSET(socketfd, &rfds)) {
          return 1;
        }
        if(FD_ISSET(socketfd, &wfds)) {
          return 2;
        }
        return 0;
    };
  } while(1);

  return 0;
}

