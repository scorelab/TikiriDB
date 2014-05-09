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
 *         Query table hedder file
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>
 */

#ifndef __QTABLE_H__
#define __QTABLE_H__

#include <stdint.h>
#include "net/rime.h"

#define SLOT_FREE 0
#define SLOT_USED 1

#define QUERY_STOPPED  0
#define QUERY_RUNNING 1


typedef struct qtable_entry {
  uint8_t qstatus:2; /* 1 */
  uint8_t slot_status:2; /* 1 */
  uint8_t qid; /* 2 */
  uint8_t qtype; /* 3 */
  void * qptr; /* 5 */
  rimeaddr_t qroot;  /* Address of query root - 7*/
  
} qtable_entry_t;

qtable_entry_t * add_query_entry(uint8_t qid, uint8_t qtype, void * qptr, rimeaddr_t * qroot);

void remove_query_entry(uint8_t qid, rimeaddr_t * qroot);

qtable_entry_t * get_query_entry(uint8_t qid, rimeaddr_t * qroot);

void init_qtable();

#endif /* __QTABLE_H__ */
