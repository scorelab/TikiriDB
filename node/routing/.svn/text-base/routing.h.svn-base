/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory  
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
 *
 * $Id: tikiriarp.h,v 0.0 2009/09/26 Exp $
 */

/**
 * \file
 *         TikiriDB Routing Interface Header file
 * \author
 *         Tharindu Nanayakkara <tharindu.ucsc@gmail.com>
 */

#ifndef __ROUTING_H__
#define __ROUTING_H__

#include "net/rime/broadcast.h"
#include "net/rime/mesh.h"

#include "lib/list.h"
#include "lib/memb.h"

struct routing_conn;

struct routing_callbacks {
  void (* recv)(struct routing_conn *c, const rimeaddr_t *from);
};

struct routing_conn {
  struct broadcast_conn c;
  const struct routing_callbacks *u;
};

void routing_open(struct routing_conn *c, uint16_t channel,
	      const struct routing_callbacks *u);
	      
void routing_close(struct routing_conn *c);

int routing_send(struct routing_conn *c, const rimeaddr_t *receiver);

#endif /* __ROUTING_H__ */
/** @} */
/** @} */
