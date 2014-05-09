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
 *      Packet buffer source file.
 *      This module is taken from TinyOS's C++ implementation of serial forwarder.
 */

#include "PacketBuffer.h"

#include "pthread.h"
#include <algorithm>

//#define DEBUG_EABLE 0
//#define ERROR_EABLE 0

#ifdef DEBUG_EABLE
#include <iostream>
#define DEBUG(message) std::cout <<"["<< this->name << "] " \
                                 << message << std::endl;
#else
#define DEBUG(message)
#endif

#ifdef ERROR_EABLE
#include <iostream>
#define ERROR(message) std::cerr << "[" << this->name << "] " \
                                 << message << " : " << strerror(errno) << std::endl;
#else
#define ERROR(message)
#endif

/*----------------------------------------------------------------------------*/
PacketBuffer::PacketBuffer(int maxPackets)
{
  pthread_mutex_init(&buffer.lock, NULL);
  pthread_cond_init(&buffer.notempty, NULL);
  pthread_cond_init(&buffer.notfull, NULL);
  this->maxPackets = maxPackets;
  this->name = "";
}
/*----------------------------------------------------------------------------*/
PacketBuffer::PacketBuffer(std::string name, int maxPackets)
{
  pthread_mutex_init(&buffer.lock, NULL);
  pthread_cond_init(&buffer.notempty, NULL);
  pthread_cond_init(&buffer.notfull, NULL);
  this->maxPackets = maxPackets;
  this->name = name;
}

/*----------------------------------------------------------------------------*/
PacketBuffer::~PacketBuffer()
{
  pthread_cond_destroy(&buffer.notempty);
  pthread_cond_destroy(&buffer.notfull);
  pthread_mutex_destroy(&buffer.lock);
}

// clears the buffer
void
PacketBuffer::clear() {
  pthread_testcancel();
  pthread_mutex_lock(&buffer.lock);
  // clear
  buffer.container.clear();
  DEBUG("PacketBuffer::clear : cleared buffer and signal <notfull>")
  pthread_cond_signal(&buffer.notfull);
  pthread_mutex_unlock(&buffer.lock);
}
/*----------------------------------------------------------------------------*/
// gets a packet from the buffer (NULL = buffer empty)
Packet
PacketBuffer::dequeue()
{
  Packet packet;
  pthread_testcancel();
  pthread_cleanup_push((void(*)(void*)) pthread_mutex_unlock, (void *) &buffer.lock);
  pthread_mutex_lock(&buffer.lock);
  // wait until buffer is _not_ empty
  while(buffer.container.size() == 0)
  {
      DEBUG("PacketBuffer::dequeue : waiting until buffer is <notempty>")
      pthread_cond_wait(&buffer.notempty, &buffer.lock);
  }
  // dequeue
  packet = buffer.container.front();
  buffer.container.pop_front();
  DEBUG("PacketBuffer::dequeue : get from buffer and signal <notfull>")
  pthread_cond_signal(&buffer.notfull);
  pthread_cleanup_pop(1);
  return packet;
}
/*----------------------------------------------------------------------------*/
// puts a packet into buffer... (SUCCESS = true)
bool PacketBuffer::enqueueFront(Packet &pPacket)
{
    pthread_testcancel();
    pthread_cleanup_push((void(*)(void*)) pthread_mutex_unlock, (void *) &buffer.lock);
    pthread_mutex_lock(&buffer.lock);
    // wait until buffer is _not_ full
    while(buffer.container.size() >= maxPackets)
    {
        DEBUG("PacketBuffer::enqueueFront : waiting until buffer is <notfull>")
        pthread_cond_wait(&buffer.notfull, &buffer.lock);
    }
    // enqueue
    buffer.container.push_front(pPacket);
    DEBUG("PacketBuffer::enqueueFront : put in buffer and signal <notempty>")
    // signal that buffer is now not empty
    pthread_cond_signal(&buffer.notempty);
    pthread_cleanup_pop(1);
    return true;
}
/*----------------------------------------------------------------------------*/
// puts a packet into buffer... (SUCCESS = true)
bool PacketBuffer::enqueueBack(Packet &pPacket)
{
    pthread_testcancel();
    pthread_cleanup_push((void(*)(void*)) pthread_mutex_unlock, (void *) &buffer.lock);
    pthread_mutex_lock(&buffer.lock);
    // wait until buffer is _not_ full
    while(buffer.container.size() >= maxPackets)
    {
        DEBUG("PacketBuffer::enqueueBack : waiting until buffer is <notfull>")
        pthread_cond_wait(&buffer.notfull, &buffer.lock);
    }
    // enqueue
    buffer.container.push_back(pPacket);
    DEBUG("PacketBuffer::enqueueBack : put in buffer and signal <notempty>")
    // signal that buffer is now not empty
    pthread_cond_signal(&buffer.notempty);
    pthread_cleanup_pop(1);
    return true;
}
/*----------------------------------------------------------------------------*/
/* checks if packet buffer is full */
bool PacketBuffer::isFull() {
  bool isFull = true;
  pthread_testcancel();
  pthread_mutex_lock(&buffer.lock);
  if (buffer.container.size() < maxPackets) {
      isFull = false;
  }
  pthread_mutex_unlock(&buffer.lock);
  return isFull;
}
/*----------------------------------------------------------------------------*/
/* checks if packet buffer is empty */
bool PacketBuffer::isEmpty() {
  bool isEmpty = true;
  pthread_testcancel();
  pthread_mutex_lock(&buffer.lock);
  if (buffer.container.size() > 0) {
      isEmpty = false;
  }
  pthread_mutex_unlock(&buffer.lock);
  return isEmpty;
}

