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
 *\file
 *      This file is part of the tikiridb system.
 *
 * \author
 *      Kasun Hewage <kch@ucsc.cmb.ac.lk>
 *      nayanajith Laxaman <nml@ucsc.cmb.ac.lk>
 */

#ifndef __SF_CLIENT_H__
#define __SF_CLIENT_H__

#include "socket.h"

#ifdef CONF_MAX_PKT_SIZE
#define MAX_PKT_SIZE CONF_MAX_PKT_SIZE
#else
#define MAX_PKT_SIZE 200
#endif

#define PKT_TYPE_DATA 1
#define PKT_TYPE_DEBUG 2

typedef struct packet
{
  char data[MAX_PKT_SIZE];
  int len;
  int type;
} packet_t;

/* client modes */

#define CLIENT_MODE_R  1 // read
#define CLIENT_MODE_W  2 // write
#define CLIENT_MODE_RW 3 // read + write
/* hanshaking error codes  */
#define ERR_UNKNOWN -1
#define ERR_OK 1
#define ERR_INVALID_MODE 2
#define ERR_UNSUPPORTED_VERSION 3
#define ERR_READ_CLIENTS_FULL 4
#define ERR_WRITE_CLIENTS_FULL 5

int
handshake(int sockfd, int mode);

int
send_packet(int sockfd, packet_t * packet);

int
receive_packet(int sockfd, packet_t * packet);

void
generate_dummy_packet(packet_t * packet);

void
print_packet(packet_t * packet);

void
print_rsv_packet(packet_t * packet);

int
generate_simple_query(packet_t * packet);

int
send_query(packet_t * packet, int qLength, int nepoches, char *host, int port);

#endif /* __SF_CLIENT_H__ */
