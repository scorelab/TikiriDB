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
 *      Header file of the serial communication module.
 */

#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>

#include "BaseComm.h"
#include "Packet.h"
#include "PacketBuffer.h"


#define SLIP_END             0300    /* indicates end of packet */
#define SLIP_ESC             0333    /* indicates byte stuffing */
#define SLIP_ESC_END         0334    /* SLIP_ESC SLIP_ESC_END means SLIP_END data byte */
#define SLIP_ESC_ESC         0335    /* SLIP_ESC SLIP_ESC_ESC means ESC data byte */
#define DEBUG_MAKER          0015    /* \r used to distinguish debug messages */



#ifdef CONF_MAX_MTU
#define MAX_MTU CONF_MAX_MTU
#else
#ifdef MAX_PKT_SIZE
#define MAX_MTU (MAX_PKT_SIZE * 2)
#else
#define MAX_MTU 200
#endif /* MAX_PKT_SIZE */
#endif /* CONF_MAX_MTU*/


class SerialComm : public BaseComm
{
  public:
    enum Parity {
      NONE = 1,
      ODD = 2,
      EVEN = 3,
      MARK = 4,
      SPACE = 5
    };

    enum StopBits {
      ONE = 1,
      TWO = 2
    };

    enum State {
      STATE_OK = 1,
      STATE_ESC = 2,
      STATE_RUBBISH = 3
    };
  protected:

    /* raw read buffer */
    typedef struct rawFifo {
      char queue[MAX_MTU];
      int head;
      int tail;
    } rawFifo_t;

    rawFifo_t rawfifo;

    /* temporary buffer to encode a packet to SLIP format */
    char tmpSLIPBuf[MAX_MTU];

    /* Pthread for serial reading. */
    pthread_t readerThread;

    bool readerThreadRunning;

    /* Pthread for serial writing. */
    pthread_t writerThread;

    bool writerThreadRunning;

    /* Device port of this sf. */
    std::string device;

    /* Baudrate of connected device. */
    int baudrate;

    /* Old serial port settings. */
    struct termios oldtio;

    /* Read fd set. */
    fd_set rfds;

    /* Write fd set. */
    fd_set wfds;

    /* fd for both reading and writing. */
    int serialFD;

    bool isStarted;

    /* packet buffer for read packets from serial device */
    PacketBuffer &readBuffer;

    /* packet buffer for packets to be written to the serial device */
    PacketBuffer &writeBuffer;

  private:
    // Do not allow standard constructor
    SerialComm();

  protected:

    /* Claculates crc byte-wise */
    static uint16_t byteCRC(uint8_t byte, uint16_t crc);

    /* returns tcflag of requested baudrate */
    static tcflag_t parseBaudrate(int requested);

    /* checks for messages from node - producer thread */
    void readSerial();

    /* write messages to serial / node - consumer thread */
    void writeSerial();

    int readBytes(char * buffer, int count);

    int writeBytes(char * buffer, int count);

    int setOptions(int baudrate, int databits, Parity parity,
                   StopBits stop, bool softwareHandshake, bool hardwareHandshake);

    /* Read a byte from FIFO. */
    int nextRawByte(unsigned char &nextByte);

    int readSLIPData(char *buffer, int bufLen);

    int writeSLIPData(const unsigned char *buffer, int bufLen);

    /* Needed to start pthreads. */
    friend void* serialReadThread(void* ob);

    friend void* serialWriteThread(void* ob);


public:
    SerialComm(const std::string device, int pBaudrate,
                                         PacketBuffer &readBuffer,
                                         PacketBuffer &writeBuffer);

    ~SerialComm();

    /* Start read and write threads. */
    int start();

    /* Cancels all running threads */
    void cancel();

    std::string getDevice() const;

    int getBaudRate() const;

    bool isRunning() const;

};

#endif // SERIALCOMM_H
