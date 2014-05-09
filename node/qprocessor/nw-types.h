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
 *         Network data types and prototypes of their conversion functions.
 *         These data types and functions are based on Network Types which is 
 *         an extension to the nesC programming language designed to support 
 *         heterogeneous networking.
 *
 *         Network Types:
 *                       http://nescc.sourceforge.net/networktypes/index.html
 *
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>       
 *         
 */

#ifndef __NW_TYPES_H__
#define __NW_TYPES_H__

#include <stdint.h>

/* Signed data types. */

typedef struct {
  char data[1]; 
} nw_int8_t;

typedef struct { 
  char data[2]; 
} nw_int16_t;

typedef struct {
 char data[4]; 
} nw_int32_t;

typedef struct {
 char data[8]; 
} nw_int64_t;

/* Unsigned data types. */

typedef struct {
 unsigned char data[1]; 
} nw_uint8_t;

typedef struct {
 unsigned char data[2]; 
} nw_uint16_t;

typedef struct { 
  unsigned char data[4]; 
} nw_uint32_t;

typedef struct {
 unsigned char data[8]; 
} nw_uint64_t;


/* 8-bits */

inline uint8_t ntoh_uint8(void * source);

inline uint8_t hton_uint8(void * target, uint8_t value); 

inline uint8_t ntoh_leuint8(void * source); 

inline uint8_t hton_leuint8(void * target, uint8_t value);

/* 16-bits */

inline uint16_t hton_uint16(void * target, uint16_t value); 

inline uint16_t ntoh_uint16(void * source);

inline uint16_t ntoh_leuint16(void * source); 

inline uint16_t hton_leuint16(void * target, uint16_t value); 

/* 32-bits */

inline uint32_t ntoh_uint32(void * source); 

inline uint32_t hton_uint32(void * target, uint32_t value); 

inline uint32_t ntoh_leuint32(void * source); 

inline uint32_t hton_leuint32(void * target, uint32_t value); 

/* 64-bits */
/*
inline uint64_t ntoh_uint64(void * source); 

inline uint64_t hton_uint64(void * target, uint64_t value);

inline uint64_t ntoh_leuint64(void * source);

inline uint64_t hton_leuint64(void * target, uint64_t value); 
*/
#endif /* __NW_TYPES_H__ */
