#ifndef __SERIAL_H__
#define __SERIAL_H__

typedef struct serial_source {
  int fd;
  int non_blocking;
} serial_source_t;

#endif /* __SERIAL_H__ */
