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
 *      Implementation of the basic file descriptor access module.
 */


#include "BaseComm.h"

#include <stddef.h>
#include <unistd.h>
#include <sys/select.h>

#include <cstdio>
#include <climits>

/*---------------------------------------------------------------------------*/
BaseComm::BaseComm()
{

}
/*---------------------------------------------------------------------------*/
BaseComm::~BaseComm()
{

}
/*---------------------------------------------------------------------------*/
int
BaseComm::fdWait(int fd, int mask, unsigned int mSecs)
{
  fd_set rfds, wfds;
  struct timeval tv;
  int retval;

  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  if(fd < 0) {
    return -1;
  }

  if((mask & 1) == 1) {
    FD_SET(fd, &rfds);
  }

  if((mask & 2) == 2) {
    FD_SET(fd, &wfds);
  }

  tv.tv_sec = mSecs / 1000;
  tv.tv_usec = mSecs % 1000;

  do {
    retval =  select(fd + 1, &rfds, &wfds,NULL,&tv);
    switch(retval) {
      case 0:  /* time out */
        return 0;

      case (-1):  /* socket error */
        return -1;

      default:
        if(FD_ISSET(fd, &rfds)) {
          return 1;
        }
        if(FD_ISSET(fd, &wfds)) {
          return 2;
        }
        return 0;
    };
  } while(1);

  return 0;
}
/*---------------------------------------------------------------------------*/
uint64_t
BaseComm::readAll(int fd, char * buffer, uint64_t count)
{
  uint64_t r;
  int n;
  char done;
  unsigned int k;

  r = 0;
  done = 0;
  while(!done) {

    /* How many bytes we read in this iteration */
    k = ((uint32_t)(count - r) > INT_MAX) ? INT_MAX : (int)(count - r);
    //log_debug("k : %lu", k);
    n = read(fd, buffer + r, k);
    if(n <= 0){
      break;
    }
    r += n;
    done = ((uint32_t)(count - r) > 0) ? 0 : 1;
  }

  return r;
}
/*---------------------------------------------------------------------------*/
uint64_t
BaseComm::writeAll(int fd, const char *buffer, uint64_t count)
{
  uint64_t s;
  int n, k;
  char done;

  s = 0;
  done = 0;
  while(!done) {

    /* How many bytes we send in this iteration */
    k = ((uint32_t)(count - s) > INT_MAX) ? INT_MAX : (int)(count - s);
    n = write(fd, buffer + s, k);

    if(n < 0){
      break;
    }
    s += n;
    done = ((uint32_t)(count - s) > 0) ? 0 : 1;
  }
  return s;
}
