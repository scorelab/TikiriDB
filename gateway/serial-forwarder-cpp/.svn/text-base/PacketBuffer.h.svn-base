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
 *      Packet buffer header file.
 *      This module is taken from TinyOS's C++ implementation of serial forwarder.
 */

#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include <pthread.h>
#include <list>
#include <string>
#include "Packet.h"

class PacketBuffer
{
  protected:

    int maxPackets;
    std::string name;

    typedef std::list<Packet> container_t;

    // thread safe buffer
    typedef struct sharedBuffer
    {
      // mutex lock for any of this vars
      pthread_mutex_t lock;
      // notempty cond
      pthread_cond_t notempty;
      // not full cond
      pthread_cond_t notfull;
     // actual buffer
      container_t container;
    } sharedBuffer_t;

    sharedBuffer_t buffer;

  public:
    PacketBuffer(int maxPackets);

    PacketBuffer(std::string name, int maxPackets);

    ~PacketBuffer();

    void clear();

    Packet dequeue();

    bool enqueueFront(Packet &pPacket);

    bool enqueueBack(Packet &pPacket);

    bool isFull();

    bool isEmpty();

};

#endif /* PACKETBUFFER_H */
