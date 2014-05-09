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
 *         Memory allocator for queries. The implementation is based on malloc
 *         implementation of avr-libc
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>      
 *         
 */

#include "qmalloc.h"
#include <stdio.h>

/* Query memory size. */
#ifndef CONF_QMEM_SIZE
#define QMEM_SIZE  256
#else
#define QMEM_SIZE CONF_QMEM_SIZE
#endif

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

struct freelist {
  size_t size;
  struct freelist *next;
};

/*---------------------------------------------------------------------------*/
static struct freelist *free_list;
static char qmem[QMEM_SIZE];
/*---------------------------------------------------------------------------*/
void
init_qmalloc()
{
  free_list = (struct freelist *)&qmem[0];
  free_list->next = NULL;
  free_list->size = QMEM_SIZE;
  PRINTF("sizeof(size_t) => %li\n", sizeof(size_t));
  PRINTF("sizeof(struct freelist) => %li\n", sizeof(struct freelist));
  PRINTF("free_list => %li\n", free_list);
  PRINTF("qmem[0] => %li\n", &qmem[0]);
}
/*---------------------------------------------------------------------------*/
void *
qmalloc(size_t len)
{
  struct freelist *flp1, *flp2;
  char *cp;
  size_t s;
  /*
   * Our minimum chunk size is the size of a pointer (plus the
   * size of the "size" field, but we don't need to account for
   * this), otherwise we could not possibly fit a freelist entry
   * into the chunk later.
   */
  if (len < sizeof(struct freelist) - sizeof(size_t)) {
    len = sizeof(struct freelist) - sizeof(size_t);
  }

  /*
   * Step 1:
   * Walk the free list and try finding a chunk that would match exactly.  
   * If we found one, we are done.  While walking, note down the size of the 
   * largest chunk we found that would still fit the request -- we need it for 
   * step 2.
   */
  flp1 = free_list;
  flp2 = NULL;
  for(s = 0 ; flp1 ;flp2 = flp1, flp1 = flp1->next) {

    if(flp1->size == len) {
      /*
       * Found it. Disconnect the chunk from the freelist and return it.
       */
      if(flp2) {
        flp2->next = flp1->next;
      } else {
        free_list = flp1->next;
      }
      return &(flp1->next);
    }

    if(flp1->size > len) {
      if (s == 0 || flp1->size < s) {
        s = flp1->size;
      }
  
    }
  }
  
  PRINTF("s => %d\n", s);
 /*
   * Step 2: 
   * If we found a chunk on the freelist that would fit (but was too large), 
   * look it up again and use it, since it is our closest match now.  Since the 
   * freelist entry needs to be split into two entries then, watch out that the
   * difference between the requested size and the size of the chunk found is 
   * large enough for another freelist entry; if not, just enlarge the request 
   * size to what we have found, and use the entire chunk.
   */
  if(s) {
    if(s - len < sizeof(struct freelist)) {
      len = s;
    }
    flp1 = free_list;
    flp2 = NULL;
    for( ; flp1; flp2 = flp1, flp1 = flp1->next) {   
      if(flp1->size == s) {
        if(len == s) {
          /*
           * Use entire chunk; same as above.
           */
          if(flp2) {
            flp2->next = flp1->next;
          } else {
            free_list = flp1->next;
          }
          return &(flp1->next);
        }
        /*
	 * Split them up.  Note that we leave the first part as the new (smaller)
         * freelist entry, and return the upper portion to the caller. This saves 
         * us the work to fix up the freelist chain; we just need to fixup the 
         * size of the current entry, and note down the size of the new chunk 
         * before returning it to the caller.
         */
        PRINTF("len => %d, s => %d flp1 => %li, flp2 => %li\n", len, s, flp1, flp2);
        cp = (char *)flp1;
        s -= len;
        cp += s - sizeof(size_t);
        flp2 = (struct freelist *)cp;
        flp2->size = len;
        flp1->size = s - sizeof(size_t);
        return &(flp2->next);
      }
    }
  }
  /*
   * We could not find a matching space. Return NULL.
   */
  return NULL;
}
/*---------------------------------------------------------------------------*/
void
qfree(void * ptr) 
{
  struct freelist *flp1, *flp2, *flp_new;
  char *cp1, *cp2, *cp_new;

  if(ptr == NULL) {
    return;
  }
  cp_new = (char *)ptr;
  cp_new -= sizeof(size_t);
  flp_new = (struct freelist *)cp_new;
  flp_new->next = NULL;

  /*
   * Trivial case first: 
   * If there's no freelist yet (Whole memory was allocated previously), 
   * our entry will be the only one on it.
   */
  if (free_list == 0) {
    free_list = flp_new;
    return;
  }
  /*
   * Now, find the position where our new entry belongs onto the freelist.  
   * Try to aggregate the chunk with adjacent chunks if possible.
   */
  flp1 = free_list;
  flp2 = NULL;
  for( ; flp1 ;flp2 = flp1, flp1 = flp1->next) {
    if(flp1 < flp_new) {
      continue;
    }
    cp1 = (char *)flp1;
    flp_new->next = flp1;
    if ((char *)&(flp_new->next) + flp_new->size == cp1) {
      /* upper chunk adjacent, assimilate it */
      flp_new->size += flp1->size + sizeof(size_t);
      flp_new->next = flp1->next;
    }
    if (flp2 == 0) {
      /* new head of freelist */
      free_list = flp_new;
      return;
    }
    break;
  }

  /*
   * Note that we get here either if we hit the "break" above, or if we fell off 
   * the end of the loop.  The latter means we've got a new topmost chunk.  
   * Either way, try aggregating with the lower chunk if possible.
   */
   flp2->next = flp_new;
   cp2 = (char *)&(flp2->next);
   if (cp2 + flp2->size == cp_new) {
     /* lower junk adjacent, merge */
     flp2->size += flp_new->size + sizeof(size_t);
     flp2->next = flp_new->next;
   }
}
/*---------------------------------------------------------------------------*/
/*
int
main(void)
{
  char *c, *d;

  init_qmalloc();
  c = qmalloc(10);
  printf("c => %li, qmem[0] => %li\n", c, &qmem[0] );
  d = qmalloc(10);
  printf("d => %li\n", d);
  return 0;
}
*/
/*---------------------------------------------------------------------------*/
