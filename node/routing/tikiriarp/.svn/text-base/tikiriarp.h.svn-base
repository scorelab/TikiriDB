/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory - University of Colombo School of Computting.
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
 *
 * $Id: tikiriarp.h,v 0.0 2009/09/26 Exp $
 */

/**
 * \file
 *         Tikiri Ad-hoc Routing Protocol Header file
 * \author
 *         Tharindu Nanayakkara <tharindu.ucsc@gmail.com>
 */

#ifndef __TIKIRIARP_H__
#define __TIKIRIARP_H__

#include "net/rime/broadcast.h"

#include "lib/list.h"
#include "lib/memb.h"

struct tikiriarp_conn;

#define TIKIRIARP_BROADCAST_ADDR 255
#define TIKIRIARP_MULTICAST_UPPER_ADDR 250
#define TIKIRIARP_MULTICAST_LOWER_ADDR 254



#define TIKIRIARP_ATTRIBUTES   { PACKETBUF_ADDR_RECEIVER, PACKETBUF_ADDRSIZE }, \
                        BROADCAST_ATTRIBUTES


typedef struct routing_entry {
  struct routing_entry *next;
  rimeaddr_t node_addr;
  rimeaddr_t next_hop_addr;
  uint8_t cost;
} routing_entry;

typedef struct tikiriarp_pkt {
  uint8_t cost;
  uint8_t routes_len;
  routing_entry routes[];
} tikiriarp_pkt;

struct tikiriarp_callbacks {
  void (* recv)(struct tikiriarp_conn *c, rimeaddr_t *from);
};

struct tikiriarp_conn {
  struct broadcast_conn c;
  const struct tikiriarp_callbacks *u;
};

void tikiriarp_open(struct tikiriarp_conn *c, uint16_t channel,
	      const struct tikiriarp_callbacks *u);
void tikiriarp_close(struct tikiriarp_conn *c);

int tikiriarp_send(struct tikiriarp_conn *c, const rimeaddr_t *receiver);

#endif /* __TIKIRIWARP_H__ */
/** @} */
/** @} */
