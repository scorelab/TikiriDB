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
 */

/**
 * \file
 *         Query message source file. 
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>       
 *         
 */

#include "messages.h"

/*---------------------------------------------------------------------------*/
int inline
get_smessage_size(smessage_header_t * smessage_header)
{
  return sizeof(smessage_header_t) + 
         (sizeof(field_t) * smessage_header->nfields) +
         (sizeof(expression_t) * smessage_header->nexprs);
}

/*---------------------------------------------------------------------------*/
int
get_message_length(message_header_t * message_header)
{
  qmessage_header_t * qmessage_header;
  int len;

  len = sizeof(message_header_t);

  if(message_header->type == MSG_QREQUEST) {
    len += sizeof(qmessage_header_t);
    qmessage_header = (qmessage_header_t *)(message_header + 1);
    switch(qmessage_header->qtype) {
      case QTYPE_SELECT:
        return len + get_smessage_size((smessage_header_t *)(qmessage_header + 1));
      case QTYPE_CREATE:
        /* TODO: implement size calculation */
      case QTYPE_DELETE:
        /* TODO: implement size calculation */
        return 0;
    }
    return 0;
  } else if(message_header->type == MSG_QREPLY) {
    /* TODO: implement size calculation for query responses */
  }

  return 0;
}
