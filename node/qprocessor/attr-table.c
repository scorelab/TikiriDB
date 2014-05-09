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
 *         Attribute table source file. 
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>       
 *         
 */

#include "attr-table.h"
#include "stdio.h"
#include <string.h>

#ifdef CONF_ATTR_TABLE_SIZE
#define ATTR_TABLE_SIZE CONF_ATTR_TABLE_SIZE  
#else
#define ATTR_TABLE_SIZE 4
#endif

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SLOT_FREE 0
#define SLOT_USED 1

static attr_entry_t attr_table[ATTR_TABLE_SIZE];
/*---------------------------------------------------------------------------*/
int 
add_attr_entry(uint8_t type,
               int (* get_data)(attr_data_t * data_ptr),
               int (* compare_data)(attr_data_t * A, attr_data_t * B, uint8_t op))
{
  int i;
  for(i=0; i< ATTR_TABLE_SIZE; i++) {
    if(attr_table[i].slot_status == SLOT_FREE) {
      attr_table[i].slot_status = SLOT_USED;
      attr_table[i].attr_type = type;
      attr_table[i].get_data = get_data;
      attr_table[i].compare_data = compare_data;
      return i;
    }
  }
  return -1;
}
/*---------------------------------------------------------------------------*/
attr_entry_t * 
get_attr_entry(uint8_t type)
{
  int i;
  for(i=0; i< ATTR_TABLE_SIZE; i++) {
    if(attr_table[i].attr_type == type) {
      return &attr_table[i];
    }  
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
void 
remove_attr_entry(uint8_t type)
{
  int i;
  for(i=0; i< ATTR_TABLE_SIZE; i++) {
    if(attr_table[i].attr_type == type) {
      memset(&attr_table[i], 0, sizeof(attr_entry_t));
      return;
    }
  }
}
/*---------------------------------------------------------------------------*/
void 
init_attr_table()
{
  memset(attr_table, 0, sizeof(attr_table));
}
/*---------------------------------------------------------------------------*/
