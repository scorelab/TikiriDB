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
 *         Query message headers. 
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>       
 *         
 */

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

// Applications run on the base station do not need rime.h header file
#ifndef GATEWAY
#include "net/rime.h"
#else
#include "rimeaddr.h"
#endif
#include "attr-table.h"
#include "nw-types.h"

#define MSG_QREQUEST 1
#define MSG_QREPLY 2

/* Query message types */
#define QTYPE_SELECT 1
#define QTYPE_CREATE 2
#define QTYPE_DELETE 3

typedef struct message_header {
  uint8_t type; /* message type */
} message_header_t;

typedef struct rfield { 
  uint8_t id; /* Field id - 1 */
  attr_data_t data; /* -5 */
  
} rfield_t;

typedef struct qresult_header {
  uint8_t  qid;        /* Query id - 1 */
  uint8_t  type;       /* Query result type. -2 */
  uint8_t  nrfields;   /* Number of tuples in the result. -3 */
  nw_uint16_t  epoch;  /* Epoch -5 */
  rimeaddr_t nodeaddr; /* Address of query root -7 */
} qresult_header_t;

/* A fields consists with the field id and the transformation operator.*/
typedef struct field {
  uint8_t  id;          /* Field id -1 */
  uint8_t  in_result:1; /* whether the field should be included in the result. -2 */
  uint8_t  type:3;      /* data type of the filed. i.e: int, float, bytes.(Currently, not used) -2 */
  uint8_t  op:4;        /* Transformation operator.(Currently, not used). -2 */
  
} field_t;

/* Boolean operators. */
enum {
  EQ = 0,
  NEQ = 1,
  GT = 2,
  GE = 3,
  LT = 4,
  LE = 5
};

/* Boolean expressions. */
typedef struct expression {
  uint8_t  l_value_index; /* index of the left value field - 1 */
  uint8_t  op; /* operator type -2 */
  attr_data_t r_value; /* -6 */
} expression_t;

/* Message header for carrying query messages. */
typedef struct qmessage_header {
  uint8_t  qid;      /* Query id - 1 */
  uint8_t  qtype;    /* Query type. - 2 */
  rimeaddr_t qroot;  /* Address of query root -4 */
} qmessage_header_t;

/* 
 * SELECT query message header. 
 * Note: We use 8-bit variable to represent the epoch duration to avoid structure
 *       alignment issues.
 */
typedef struct smessage_header {
  uint8_t  nfields;            /* Number of total fields. - 1 */
  uint8_t  nexprs;             /* Number of expressions in WHERE clause. - 2 */
  uint8_t  in_buffer;          /* Input buffer, 0 is the default. - 3 */
  uint8_t  out_buffer;         /* Output buffer, 0 is the default. - 4 */
  nw_uint16_t  epoch_duration; /* Epoch duration in seconds. - 6 */
  nw_uint16_t  nepochs;        /* Number of epochs - 8 */
} smessage_header_t;

/* TODO: CREATE BUFFER query message header. */
   
/* TODO: DELETE BUFFER query message header. */

int get_smessage_size(smessage_header_t * smessage_header);

int get_message_length(message_header_t * message_header);

#endif /* __MESSAGES_H__ */
