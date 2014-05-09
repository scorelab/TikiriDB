/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory -
 * University of Colombo School of Computing.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the tikiridb system.
 *
 * \author
 *      Kasun Hewage <kch@ucsc.cmb.ac.lk>
 */

/* \file
 *      Implementation of the serial communication module.
 */



#include "SerialComm.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <sstream>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

//#define DEBUG_EABLE 1
#define ERROR_EABLE 1

#if DEBUG_EABLE
#include <iostream>
#define DEBUG(message) std::cout << message << std::endl;
#else
#define DEBUG(message)
#endif

#if ERROR_EABLE
#include <iostream>
#define ERROR(message) std::cerr << message << " : " << strerror(errno) << std::endl;
#else
#define ERROR(message)
#endif


using namespace std;

/* forward declarations of pthrad helper functions*/
void* serialReadThread(void*);
void* serialWriteThread(void*);

uint16_t
SerialComm::byteCRC(uint8_t byte, uint16_t crc)
{
  crc = (uint8_t)(crc >> 8) | (crc << 8);
  crc ^= byte;
  crc ^= (uint8_t)(crc & 0xff) >> 4;
  crc ^= crc << 12;
  crc ^= (crc & 0xff) << 5;
  return crc;
}
/*---------------------------------------------------------------------------*/
tcflag_t
SerialComm::parseBaudrate(int requested)
{
  int baudrate;

  switch (requested) {
#ifdef B50
  case 50:
    baudrate = B50;
    break;
#endif
#ifdef B75

  case 75:
    baudrate = B75;
    break;
#endif
#ifdef B110

  case 110:
    baudrate = B110;
    break;
#endif
#ifdef B134

  case 134:
    baudrate = B134;
    break;
#endif
#ifdef B150

  case 150:
    baudrate = B150;
    break;
#endif
#ifdef B200

  case 200:
    baudrate = B200;
    break;
#endif
#ifdef B300

  case 300:
    baudrate = B300;
    break;
#endif
#ifdef B600

  case 600:
    baudrate = B600;
    break;
#endif
#ifdef B1200

  case 1200:
    baudrate = B1200;
    break;
#endif
#ifdef B1800

  case 1800:
    baudrate = B1800;
    break;
#endif
#ifdef B2400

  case 2400:
    baudrate = B2400;
    break;
#endif
#ifdef B4800

  case 4800:
    baudrate = B4800;
    break;
#endif
#ifdef B9600

  case 9600:
    baudrate = B9600;
    break;
#endif
#ifdef B19200

  case 19200:
    baudrate = B19200;
    break;
#endif
#ifdef B38400

  case 38400:
    baudrate = B38400;
    break;
#endif
#ifdef B57600

  case 57600:
    baudrate = B57600;
    break;
#endif
#ifdef B115200

  case 115200:
    baudrate = B115200;
    break;
#endif
#ifdef B230400

  case 230400:
    baudrate = B230400;
    break;
#endif
#ifdef B460800

  case 460800:
    baudrate = B460800;
    break;
#endif
#ifdef B500000

  case 500000:
    baudrate = B500000;
    break;
#endif
#ifdef B576000

  case 576000:
    baudrate = B576000;
    break;
#endif
#ifdef B921600

  case 921600:
    baudrate = B921600;
    break;
#endif
#ifdef B1000000

  case 1000000:
    baudrate = B1000000;
    break;
#endif
#ifdef B1152000

  case 1152000:
    baudrate = B1152000;
    break;
#endif
#ifdef B1500000

  case 1500000:
    baudrate = B1500000;
    break;
#endif
#ifdef B2000000

  case 2000000:
    baudrate = B2000000;
    break;
#endif
#ifdef B2500000

  case 2500000:
    baudrate = B2500000;
    break;
#endif
#ifdef B3000000

  case 3000000:
    baudrate = B3000000;
    break;
#endif
#ifdef B3500000

  case 3500000:
    baudrate = B3500000;
    break;
#endif
#ifdef B4000000

   case 4000000:
     baudrate = B4000000;
     break;
#endif

   default:
      baudrate = 0;
  }
  return baudrate;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::setOptions(int baudrate, int databits, Parity parity,
                   StopBits stop, bool softwareHandshake, bool hardwareHandshake)
{
  struct termios newtio;
  speed_t baud;

  if(tcgetattr(serialFD, &newtio) != 0){
    ERROR("tcgetattr() 3 failed");
    return -1;
  }
  baud = parseBaudrate(baudrate);
  cfsetospeed(&newtio, (speed_t)baud);
  cfsetispeed(&newtio, (speed_t)baud);

     /* We generate mark and space parity ourself. */
  if(databits == 7 && (parity == MARK || parity == SPACE)) {
    databits = 8;
  }

  switch (databits)
  {
    case 5:
      newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS5;
      break;
    case 6:
      newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS6;
      break;
    case 7:
      newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS7;
      break;
    case 8:
    default:
      newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
      break;
   }
   newtio.c_cflag |= CLOCAL | CREAD;

   //parity
   newtio.c_cflag &= ~(PARENB | PARODD);
   if (parity == EVEN) {
     newtio.c_cflag |= PARENB;
   }
   else if (parity == ODD) {
     newtio.c_cflag |= (PARENB | PARODD);
   }
   newtio.c_cflag &= ~CRTSCTS;
   //stopbits
   if (stop == TWO) {
     newtio.c_cflag |= CSTOPB;
   }
   else {
     newtio.c_cflag &= ~CSTOPB;
   }

   newtio.c_iflag=IGNBRK;

   if (softwareHandshake) {
     newtio.c_iflag |= IXON | IXOFF;
   }
   else {
     newtio.c_iflag &= ~(IXON|IXOFF|IXANY);
   }

   newtio.c_lflag=0;
   newtio.c_oflag=0;

   newtio.c_cc[VTIME]=1;
   newtio.c_cc[VMIN]=60;

//   tcflush(m_fd, TCIFLUSH);
   if (tcsetattr(serialFD, TCSANOW, &newtio)!=0) {
     ERROR("tcsetattr() 1 failed");
     return -1;
   }

   int mcs = 0;
   ioctl(serialFD, TIOCMGET, &mcs);
   mcs |= TIOCM_RTS;
   ioctl(serialFD, TIOCMSET, &mcs);

   if (tcgetattr(serialFD, &newtio) != 0) {
     ERROR("tcgetattr() 4 failed");
      return -1;
   }

   //hardware handshake
   if (hardwareHandshake) {
     newtio.c_cflag |= CRTSCTS;
   }
   else {
     newtio.c_cflag &= ~CRTSCTS;
   }
/*  if (on)
     newtio.c_cflag |= CRTSCTS;
  else
     newtio.c_cflag &= ~CRTSCTS;*/
   // apply new settings.
   if (tcsetattr(serialFD, TCSANOW, &newtio)!=0) {
     ERROR("tcsetattr() 2 failed");
     return -1;
   }

  return 0;
}
/*---------------------------------------------------------------------------*/
SerialComm::SerialComm(std::string device,
                       int baudrate,
                       PacketBuffer &readBuffer,
                       PacketBuffer &writeBuffer) : readBuffer(readBuffer),
                                                    writeBuffer(writeBuffer)
{
  tcflag_t baudflag;
  struct termios newtio;  // Serial port setting

  this->writerThreadRunning = false;
  this->readerThreadRunning = false;
  this->isStarted = false;
  this->baudrate = baudrate;
  this->device = device;

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

}
/*---------------------------------------------------------------------------*/
SerialComm::~SerialComm()
{
  if (serialFD > 2) {
    close(serialFD);
  }

}
/*---------------------------------------------------------------------------*/
int
SerialComm::start()
{

  int retValue;
  int n;

  struct termios newtio;  // Serial port setting
  tcflag_t baudflag;


  // FIFO initialization
  rawfifo.head = 0;
  rawfifo.tail = 0;

  serialFD = open(device.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (serialFD < 0) {
    ERROR("Can not open device "<< device);
    return -1;
  }

  if (tcflush(serialFD, TCIFLUSH) < 0) {
    ERROR("Can not flush non-read input data for device "<< device);
    return -1;
  }

  if (tcflush(serialFD, TCOFLUSH) < 0) {
    ERROR("Can not flush non-read input data for device "<< device);
    return -1;
  }
  // read function call will return immediately even though no character
  // are available.
  n = fcntl(serialFD, F_GETFL, 0);
  fcntl(serialFD, F_SETFL, n & ~O_NDELAY);

  // Save old configurations
  if (tcgetattr(serialFD, &oldtio) < 0) {
    ERROR("Can not get configurations for device "<< device);
    return -1;
  }

  // set serial port options.
  retValue = setOptions(baudrate, 8, NONE, ONE, false, false);

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  retValue = pthread_create(&readerThread, NULL, serialReadThread, this);
  if (retValue < 0) {
    ERROR("Can not start reader thread. Device : "<< device);
    close(serialFD);
    return -1;
  }
  this->readerThreadRunning = true;
  retValue = pthread_create(&writerThread, NULL, serialWriteThread, this);
  if (retValue < 0) {
    ERROR("Can not start writer thread. Device : "<< device);
    close(serialFD);
    return -1;
  }
  this->writerThreadRunning = true;
  this->isStarted = true;

  return 0;
}
/*---------------------------------------------------------------------------*/
std::string
SerialComm::getDevice() const
{
  return device;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::getBaudRate() const
{
    return baudrate;
}
/*---------------------------------------------------------------------------*/
bool
SerialComm::isRunning() const
{
  return isStarted;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::readBytes(char * buffer, int count)
{
  int retValue;
  int tmpCnt = 0;

  while (tmpCnt == 0) {
    retValue = fdWait(serialFD, 1, 1000);
    if (retValue < 0) {
      return -1;
    }
    tmpCnt = read(serialFD, buffer, count);
  }

  return tmpCnt;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::writeBytes(char * buffer, int count)
{
  int sent;
  int n, k;

  sent = 0;
  k = count - sent;
  while(sent < count) {
    n = write(serialFD, buffer + sent, k);
    if(n < 0) {
      break;
    }
    sent += n;
  }
  return sent;
}
/*---------------------------------------------------------------------------*/
/* helper function to start serial reader pthread */
void*
serialReadThread(void* ob)
{
    static_cast<SerialComm*>(ob)->readSerial();
    return NULL;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::nextRawByte(unsigned char &nextByte)
{
  if (rawfifo.head < rawfifo.tail) {
    // still we have data to be processed in FIFO
    nextByte = rawfifo.queue[rawfifo.head++];
  } else {
    // no data in FIFO. need to get some bytes from serial device
    rawfifo.head = 0;
    rawfifo.tail = readBytes(rawfifo.queue, MAX_MTU);
    if (rawfifo.tail < 0) {
      return -1;
    }
    nextByte = rawfifo.queue[rawfifo.head++];
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
int
SerialComm::readSLIPData(char *buffer, int bufLen)
{
  int retValue;
  int received = 0;
  unsigned char nextByte;
  bool isDebugData = false;

  while(true) {
    retValue = nextRawByte(nextByte);
    if (retValue < 0) {
      return -1;
    }

    switch(nextByte) {
      case SLIP_END:
        if(received > 0) {
          return received;
        }
        break;
       case SLIP_ESC:
         retValue = nextRawByte(nextByte);
         if (retValue < 0) {
           return -1;
         }

         switch(nextByte) {
           case SLIP_ESC_END:
             nextByte = SLIP_END;
             break;
           case SLIP_ESC_ESC:
             nextByte = SLIP_ESC;
             break;
           case DEBUG_MAKER:
             nextByte = DEBUG_MAKER;
             break;
           default:
             DEBUG("SerialComm::readSLIPData : protocol error.");
         }

       default:
         if(received < bufLen) {
           buffer[received++] = nextByte;
         }
    }

  }
  return 0;
}
/*
 * Though it is not necessary have SLIP_END at the beging it is recommended to
 * avoid any erroneous bytes which have been caused by line noise.
 *
 */
/*---------------------------------------------------------------------------*/
int
SerialComm::writeSLIPData(const unsigned char *buffer, int bufLen)
{
  int i;
  int k;
  int finalLen = 0;

  tmpSLIPBuf[finalLen++] = SLIP_END;
  for(i=0; i < bufLen; i++) {
    switch(buffer[i]) {
      case SLIP_END:
        tmpSLIPBuf[finalLen++] = SLIP_ESC;
        tmpSLIPBuf[finalLen++] = SLIP_ESC_END;
        break;
      case SLIP_ESC:
        tmpSLIPBuf[finalLen++] = SLIP_ESC;
        tmpSLIPBuf[finalLen++] = SLIP_ESC_ESC;
        break;
      default:
        tmpSLIPBuf[finalLen++] = buffer[i];
        break;
    }
  }
  tmpSLIPBuf[finalLen++] = SLIP_END;
  DEBUG("SerialComm::writeSLIPData : final length :"<<finalLen);
  k = writeAll(serialFD, tmpSLIPBuf, finalLen);
  if(k != finalLen) {
    ERROR("SerialComm::sendSLIPData : error sending SLIP data");
    return -1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
/* read from serial device */
void
SerialComm::readSerial()
{
  int received;
  char buffer[MAX_MTU];
  int type;

  while(true) {
    received = readSLIPData(buffer, MAX_MTU);
    if(received < 0) {
      DEBUG("SerialComm::readSerial : warning error on reading SLIP data");
    } else {
      Packet packet;
      if(buffer[0] == DEBUG_MAKER) {
        type = PKT_TYPE_DEBUG;
#if DEBUG_EABLE
        fwrite(buffer + 1, received - 1, 1, stderr);
#endif /* #if DEBUG_EABLE */
      } else {
        type = PKT_TYPE_DATA;
      }
      packet.setPayload(buffer, received, type);
      if(!readBuffer.isFull()) {
        readBuffer.enqueueBack(packet);
      } else {
        DEBUG("SerialComm::readSerial : warning! read buffer full. Dropping the packet");
      }

    }
  }

}
/*---------------------------------------------------------------------------*/
/* helper function to start serial writer pthread */
void*
serialWriteThread(void* ob)
{
  static_cast<SerialComm*>(ob)->writeSerial();
  return NULL;
}
/*---------------------------------------------------------------------------*/
/* writes to serial/node */
void
SerialComm::writeSerial()
{
  Packet packet;
  int k;
  while(true) {
    packet = writeBuffer.dequeue();
    k = writeSLIPData((const unsigned char *)packet.getPayload(), packet.getPacketLength());
    if(k < 0) {
      DEBUG("SerialComm::writeSerial : warning error on writing SLIP data");
    }
  }
}
/*---------------------------------------------------------------------------*/
/* cancels all running threads */
void
SerialComm::cancel()
{
  pthread_t callingThread = pthread_self();
  if(readerThreadRunning && pthread_equal(callingThread, readerThread)) {
    DEBUG("SerialComm::cancel : by readerThread");
    pthread_detach(readerThread);

    if (writerThreadRunning) {
      pthread_cancel(writerThread);
      DEBUG("SerialComm::cancel : writerThread canceled, joining")
      pthread_join(writerThread, NULL);
      writerThreadRunning = false;
    }
    readerThreadRunning = false;

    pthread_exit(NULL);
  }
  else if (writerThreadRunning && pthread_equal(callingThread, writerThread)) {
    DEBUG("SerialComm::cancel : by writerThread")
    pthread_detach(writerThread);
    if (readerThreadRunning) {
      pthread_cancel(readerThread);
      DEBUG("SerialComm::cancel : readerThread canceled, joining")
      pthread_join(readerThread, NULL);
      readerThreadRunning = false;
    }
    writerThreadRunning = false;

    pthread_exit(NULL);
  }
  else {
    DEBUG("SerialComm::cancel : by other thread")
    if (readerThreadRunning) {
      pthread_cancel(readerThread);
      DEBUG("SerialComm::cancel : readerThread canceled, joining")
      pthread_join(readerThread, NULL);
      readerThreadRunning = false;
    }
    if (writerThreadRunning) {
      pthread_cancel(writerThread);
      DEBUG("SerialComm::cancel : writerThread canceled, joining")
      pthread_join(writerThread, NULL);
      writerThreadRunning = false;
    }

  }
}

