/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory,
 * University of Colombo School of Computting.
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
 */

/**
 * \file
 *         Query table source file
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>
 */

#include "qtable.h"
#include <stdio.h>
#include <string.h>

#ifdef CONF_QTABLE_SIZE
#define QTABLE_SIZE CONF_QTABLE_SIZE  
#else
#define QTABLE_SIZE 4
#endif

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static qtable_entry_t qtable[QTABLE_SIZE];
/*---------------------------------------------------------------------------*/
qtable_entry_t * 
add_query_entry(uint8_t qid, uint8_t qtype, void * qptr, rimeaddr_t * qroot)
{
  int i;
  for(i=0; i< QTABLE_SIZE; i++) {
    if(qtable[i].slot_status == SLOT_FREE) {
      qtable[i].slot_status = SLOT_USED;
      qtable[i].qid = qid;
      qtable[i].qtype = qtype;
      qtable[i].qptr = qptr;
      rimeaddr_copy(&qtable[i].qroot, qroot);
      qtable[i].qstatus = QUERY_STOPPED;  
      PRINTF("[DEBUG]: Query added id %d\n", qid);
      return &qtable[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
void 
remove_query_entry(uint8_t qid, rimeaddr_t * qroot) 
{
  int i;
  for(i=0; i< QTABLE_SIZE; i++) {
    if(qtable[i].qid == qid && rimeaddr_cmp(&qtable[i].qroot, qroot)) {
      memset(&qtable[i], 0, sizeof(qtable_entry_t));
      return;
    }
  }
}
/*---------------------------------------------------------------------------*/
qtable_entry_t * 
get_query_entry(uint8_t qid, rimeaddr_t * qroot)
{
  int i;
  for(i=0; i< QTABLE_SIZE; i++) {
    if(qtable[i].qid == qid && rimeaddr_cmp(&qtable[i].qroot, qroot)) {
      return &qtable[i];
    }
  } 
  return NULL;
}
/*---------------------------------------------------------------------------*/
void 
init_qtable()
{
  memset(qtable, 0, sizeof(qtable));
}
/*---------------------------------------------------------------------------*/
