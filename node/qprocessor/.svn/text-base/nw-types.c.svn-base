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
 *         Implementation of the network data types conversion functions. 
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>       
 *         
 */

#include "nw-types.h"

/*---------------------------------------------------------------------------*/
/* 8-bits */

inline uint8_t 
ntoh_uint8(void * source)
{
  uint8_t *base = source;
  return base[0];
}
/*---------------------------------------------------------------------------*/
inline uint8_t 
hton_uint8(void * target, uint8_t value) 
{
  uint8_t *base = target;
  base[0] = value;
  return value;
}
/*---------------------------------------------------------------------------*/
inline uint8_t 
ntoh_leuint8(void * source) 
{
  uint8_t *base = source;
  return base[0];
}
/*---------------------------------------------------------------------------*/
inline uint8_t 
hton_leuint8(void * target, uint8_t value) 
{
  uint8_t *base = target;
  base[0] = value;
  return value;
}
/*---------------------------------------------------------------------------*/
/* 16-bits */

inline uint16_t 
hton_uint16(void * target, uint16_t value) 
{
  uint8_t *base = target;
  base[1] = value;
  base[0] = value >> 8;
  return value;
}
/*---------------------------------------------------------------------------*/
inline uint16_t 
ntoh_uint16(void * source)
{
  uint8_t *base = source;
  return (uint16_t)(base[0] << 8 | base[1]);
}
/*---------------------------------------------------------------------------*/
inline uint16_t 
ntoh_leuint16(void * source) 
{
  uint8_t *base = source;
  return (uint16_t)base[1] << 8 | base[0];
}
/*---------------------------------------------------------------------------*/
inline uint16_t 
hton_leuint16(void * target, uint16_t value) 
{
  uint8_t *base = target;
  base[0] = value;
  base[1] = value >> 8;
  return value;
}
/*---------------------------------------------------------------------------*/
/* 32-bits */

inline uint32_t 
ntoh_uint32(void * source) 
{
  uint8_t *base = source;
  return (uint32_t)base[0] << 24 |
         (uint32_t)base[1] << 16 |
         (uint32_t)base[2] << 8 | base[3];
}
/*---------------------------------------------------------------------------*/
inline uint32_t 
hton_uint32(void * target, uint32_t value) 
{
  uint8_t *base = target;
  base[3] = value;
  base[2] = value >> 8;
  base[1] = value >> 16;
  base[0] = value >> 24;
  return value;
}
/*---------------------------------------------------------------------------*/
inline uint32_t 
ntoh_leuint32(void * source) 
{
  uint8_t *base = source;
  return (uint32_t)base[3] << 24 |
         (uint32_t)base[2] << 16 |
         (uint32_t)base[1] << 8 | base[0];
}
/*---------------------------------------------------------------------------*/
inline uint32_t 
hton_leuint32(void * target, uint32_t value) 
{
  uint8_t *base = target;
  base[0] = value;
  base[1] = value >> 8;
  base[2] = value >> 16;
  base[3] = value >> 24;
  return value;
}
/*---------------------------------------------------------------------------*/
/* 64-bits */
/*
inline uint64_t 
ntoh_uint64(void * source) 
{
  uint8_t *base = source;
  return (uint64_t)base[0] << 56 |
         (uint64_t)base[1] << 48 |
         (uint64_t)base[2] << 40 |
         (uint64_t)base[3] << 32 |
         (uint64_t)base[4] << 24 |
         (uint64_t)base[5] << 16 |
         (uint64_t)base[6] << 8  | base[7];
} 


inline uint64_t 
hton_uint64(void * target, uint64_t value)
{
  uint8_t *base = target;
  base[7] = value;
  base[6] = value >> 8;
  base[5] = value >> 16;
  base[4] = value >> 24;
  base[3] = value >> 32;
  base[2] = value >> 40;
  base[1] = value >> 48;
  base[0] = value >> 56;
  return value;
}

inline uint64_t 
ntoh_leuint64(void * source)
{
  uint8_t *base = source;
  return (uint64_t)base[7] << 56 |
         (uint64_t)base[6] << 48 |
         (uint64_t)base[5] << 40 |
         (uint64_t)base[4] << 32 |
         (uint64_t)base[3] << 24 |
         (uint64_t)base[2] << 16 |
         (uint64_t)base[1] << 8  | base[0];
}

inline uint64_t 
hton_leuint64(void * target, uint64_t value) 
{
  uint8_t *base = target;
  base[0] = value;
  base[1] = value >> 8;
  base[2] = value >> 16;
  base[3] = value >> 24;
  base[4] = value >> 32;
  base[5] = value >> 40;
  base[6] = value >> 48;
  base[7] = value >> 56;
  return value;
}
*/

