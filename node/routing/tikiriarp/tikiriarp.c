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
 * $Id: tikiriarp.c,v 0.0 2009/09/26 Exp $
 */

/**
 * \file
 *         Tikiri Ad-hoc Routing Protocol Source file
 * \author
 *         Tharindu Nanayakkara <tharindu.ucsc@gmail.com>
 */

#include "net/rime.h"
#include "tikiriarp.h"
#include <string.h>

static const struct packetbuf_attrlist attributes[] =
  {
    TIKIRIARP_ATTRIBUTES
    PACKETBUF_ATTR_LAST
  };

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) printf(__VA_ARGS__)
#endif

/*---------------------------------------------------------------------------*/
static void
recv_from_broadcast(struct broadcast_conn *broadcast, rimeaddr_t *from)
{
  struct tikiriarp_conn *c = (struct tikiriarp_conn *)broadcast;
  
  //rimeaddr_t *recv_addr = (rimeaddr_t *) paketbuf_addr(PACKETBUF_ADDR_RECEIVER);

  PRINTF("%d.%d: bc: recv_from_broadcast, receiver %d.%d\n",
	 rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],
	 packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[0],
	 packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[1]);
	 
  if(packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[0] == TIKIRIARP_BROADCAST_ADDR) {
	  c->u->recv(c,from);
	} else if(rimeaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER), &rimeaddr_node_addr)) {
    c->u->recv(c, from);
  }
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc = {recv_from_broadcast};

/*---------------------------------------------------------------------------*/
void
tikiriarp_open(struct tikiriarp_conn *c, uint16_t channel,
	     const struct tikiriarp_callbacks *u)
{
  broadcast_open(&c->c, channel, &bc);
  c->u = u;
  channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void
tikiriarp_close(struct tikiriarp_conn *c)
{
  broadcast_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int
tikiriarp_send(struct tikiriarp_conn *c, const rimeaddr_t *receiver)
{
  PRINTF("%d.%d: tikiriarp_send to %d.%d\n",
	 rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1],
	 receiver->u8[0], receiver->u8[1]);
  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, receiver);
  return broadcast_send(&c->c);
}
/*---------------------------------------------------------------------------*/
/** @} */
