#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

#include "serial.h"

/*---------------------------------------------------------------------------*/
static tcflag_t 
parse_baudrate(int requested)
{
  int baudrate;

  switch (requested) {
#ifdef B50
    case 50: baudrate = B50; break;
#endif
#ifdef B75
    case 75: baudrate = B75; break;
#endif
#ifdef B110
    case 110: baudrate = B110; break;
#endif
#ifdef B134
    case 134: baudrate = B134; break;
#endif
#ifdef B150
    case 150: baudrate = B150; break;
#endif
#ifdef B200
    case 200: baudrate = B200; break;
#endif
#ifdef B300
    case 300: baudrate = B300; break;
#endif
#ifdef B600
    case 600: baudrate = B600; break;
#endif
#ifdef B1200
    case 1200: baudrate = B1200; break;
#endif
#ifdef B1800
    case 1800: baudrate = B1800; break;
#endif
#ifdef B2400
    case 2400: baudrate = B2400; break;
#endif
#ifdef B4800
    case 4800: baudrate = B4800; break;
#endif
#ifdef B9600
    case 9600: baudrate = B9600; break;
#endif
#ifdef B19200
    case 19200: baudrate = B19200; break;
#endif
#ifdef B38400
    case 38400: baudrate = B38400; break;
#endif
#ifdef B57600
    case 57600: baudrate = B57600; break;
#endif
#ifdef B115200
    case 115200: baudrate = B115200; break;
#endif
#ifdef B230400
    case 230400: baudrate = B230400; break;
#endif
#ifdef B460800
    case 460800: baudrate = B460800; break;
#endif
#ifdef B500000
    case 500000: baudrate = B500000; break;
#endif
#ifdef B576000
    case 576000: baudrate = B576000; break;
#endif
#ifdef B921600
    case 921600: baudrate = B921600; break;
#endif
#ifdef B1000000
    case 1000000: baudrate = B1000000; break;
#endif
#ifdef B1152000
    case 1152000: baudrate = B1152000; break;
#endif
#ifdef B1500000
    case 1500000: baudrate = B1500000; break;
#endif
#ifdef B2000000
    case 2000000: baudrate = B2000000; break;
#endif
#ifdef B2500000
    case 2500000: baudrate = B2500000; break;
#endif
#ifdef B3000000
    case 3000000: baudrate = B3000000; break;
#endif
#ifdef B3500000
    case 3500000: baudrate = B3500000; break;
#endif
#ifdef B4000000
    case 4000000: baudrate = B4000000; break;
#endif
    default:
      baudrate = 0;
  }
  return baudrate;
}
/*---------------------------------------------------------------------------*/
/* Opens serial port device at specified baud_rate. If non_blocking is true, 
 * read_serial_packet calls will be non-blocking (writes are always blocking, 
 * for now at least).
 */
serial_source_t * 
open_serial_source(const char *device, int baud_rate, int non_blocking)
{

  struct termios newtio;
  int fd;
  tcflag_t baudflag;

  baudflag = parse_baudrate(baud_rate);
  if (!baudflag) {
    return NULL;
  }

  fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd < 0) {
    return NULL;
  }

  /* Serial port setting */
  memset(&newtio, 0, sizeof(newtio));
  newtio.c_cflag = CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | IGNBRK;
  cfsetispeed(&newtio, baudflag);
  cfsetospeed(&newtio, baudflag);

  /* Raw output_file */
  newtio.c_oflag = 0;

  if(tcflush(fd, TCIFLUSH) >= 0 && tcsetattr(fd, TCSANOW, &newtio) >= 0) {
    serial_source_t * s_src = (serial_source_t *)malloc(sizeof(serial_source_t));
    if(s_src) {
      memset(s_src, 0, sizeof(serial_source_t));
      s_src->fd = fd;
      s_src->non_blocking = non_blocking;
      return s_src;
    }
  }

  close(fd);

  return NULL;
}
/*---------------------------------------------------------------------------*/
/* Closes serial source src
 * Returns: 0 if successful, -1 if some problem occured (but source is
 * considered closed anyway)
 */
int 
close_serial_source(serial_source_t * src)
{
  int ok = close(src->fd);
  free(src);
  return ok;
}
