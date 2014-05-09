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
 *         Query processor hedder file
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>
 */


#ifndef __QPROCESSOR_H__
#define __QPROCESSOR_H__

#include <stdint.h>
#include "net/rime.h"

#include "attr-table.h"
#include "messages.h"


typedef struct qprocessor_callbacks {
  void (* recv)(const rimeaddr_t *from);
  int (* send)(const rimeaddr_t *receiver);
} qprocessor_callbacks_t;


/* A fields consists with the field id and the transformation operator.*/
typedef struct field_data {
  uint8_t  id;          /* Field id - 1*/
  uint8_t  in_result:1; /* whether the field should be included in the result. - 1*/
  uint8_t  type:3;      /* data type of the filed. i.e: int, float, bytes.  - 1 */
  uint8_t  op:4;        /* Transformation operator. Currently, not used. - 1 */
  /* TODO: Add support for aggregated data. */
  attr_data_t data;
} field_data_t;


typedef struct expression_data {
  field_data_t * l_valuep; /* 2 */
  uint8_t  op; /* 3 */
  attr_data_t r_value; /* 8 */
} expression_data_t;


/* 
 * Simple data structure that represents a SELECT query.
 *
 */
typedef struct squery_data {
  uint8_t  qid;
  uint8_t  nfields;            /* Number of fields in SELECT clause. */
  uint8_t  nexprs;             /* Number of expressions in WHERE clause.*/
  uint8_t  in_buffer_id;       /* Input buffer ID, 0 is the default.*/
  uint8_t  out_buffer_id;      /* Output buffer ID, 0 is the default. */
  nw_uint16_t  epoch_duration; /* Epoch duration in seconds.*/
  nw_uint16_t  nepochs;        /* Number of epochs. */
  nw_uint16_t  current_epoch;  /* current epoch. */
  struct ctimer   timer;       /* time between two ctimer events is epoch period. */
} squery_data_t;


int qprocessor_init(qprocessor_callbacks_t * callbacks);

#endif /* __QPROCESSOR_H__ */
