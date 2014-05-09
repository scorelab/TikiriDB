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
 *      Implementation of the Packet.
 */

#include "Packet.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
using namespace std;

#define get_random(max) rand() % max

/*----------------------------------------------------------------------------*/
Packet::Packet()
{
}
/*----------------------------------------------------------------------------*/
Packet::Packet(const Packet &packet)
{
  this->bufLen = packet.bufLen;
  setPayload(packet.getPayload(), packet.bufLen, packet.getPacketType());
}

/*----------------------------------------------------------------------------*/
const char *
Packet::getPayload() const
{
  return buffer;
}
/*----------------------------------------------------------------------------*/
const int
Packet::getPacketType() const
{
  return type;
}
/*----------------------------------------------------------------------------*/
const int
Packet::getMaxPacketSize() const
{
  return maxPacketLength;
}
/*----------------------------------------------------------------------------*/
bool
Packet::setPayload(const char *srcBuffer, int len, int type)
{
  int ret;

  if(this->maxPacketLength < len) {
    return false;
  }

  memcpy(this->buffer, srcBuffer, len);
  this->bufLen = len;
  this->type = type;
  return true;
}
/*----------------------------------------------------------------------------*/
const int
Packet::getPacketLength() const
{
  return this->bufLen;
}
/*----------------------------------------------------------------------------*/
void
Packet::fillDummyData()
{
  int i;

  srand(time(NULL));

  bufLen = get_random(Packet::getMaxPacketSize());

  for(i = 0; i<bufLen; i++) {
    buffer[i] = get_random(254);
  }

}
void
Packet::printPacket()
{
#define ROW_COUNT 16

  int i, k, printed, left, end;
  int nrows;

  for(i=0; i< 2 * ROW_COUNT; i++) {
    printf("=");
  }
  printf("\n");
  printf("length : %d\n", bufLen);
  printf("data   :\n");
  nrows = bufLen / ROW_COUNT;
  for(i = 0; i <= nrows; i++) {
    printf("------- ");
    printed = ROW_COUNT * i;
    left = bufLen - printed;
    end = printed + ((left > ROW_COUNT) ? ROW_COUNT : left);
    for(k = printed; k < end; k++) {
      printf("%02X ", (unsigned char)buffer[k]);
    }
    printf("\n");
  }
  fflush(stdout);
}
/*----------------------------------------------------------------------------*/
Packet::~Packet()
{
  this->bufLen = 0;
}
