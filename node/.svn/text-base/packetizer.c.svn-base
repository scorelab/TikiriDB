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
 *         Packetizer Source file
 * \author
 *         Kasun Hewage <kch@ucsc.cmb.ac.lk>        
 */

#include "slip.h"
#include "packetizer.h"
#include <string.h> /* for memcpy() */

#define TRUE 1
#define FALSE 0

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#ifdef PLATFORM_AVR
#include <avr/pgmspace.h>
#define PRINTF(_fmt_, ...) printf_P(PSTR(_fmt_), ##__VA_ARGS__)
#else /* PLATFORM_AVR */
#define PRINTF(...) printf(__VA_ARGS__)
#endif /* PLATFORM_AVR */
#else /* DEBUG  */
#define PRINTF(...)
#endif

/* SLIP character types */

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

/* SLIP states */

enum {
  STATE_TWOPACKETS = 0,	/* We have 2 packets and drop incoming data. */
  STATE_OK = 1,
  STATE_ESC = 2,
  STATE_RUBBISH = 3,
  STATE_PROCESSING = 4,
};


#define PACKETIZER_BUF_SIZE 128

static u8_t state;
static u8_t rxbuf[PACKETIZER_BUF_SIZE];
static uint8_t data_len;

process_event_t packet_data_event_message;

PROCESS_NAME(tikiridb_process);
PROCESS(packetizer_process, "Serial packetizer");

/*---------------------------------------------------------------------------*/
int 
packetizer_input_byte(unsigned char c) 
{
  switch(state) {
  case STATE_PROCESSING:
    return 0;
  case STATE_RUBBISH:
    if(c == SLIP_END) {
      state = STATE_OK;
    }
    return 0;
    
  case STATE_ESC:
    if(c == SLIP_ESC_END) {
      c = SLIP_END;
    } else if(c == SLIP_ESC_ESC) {
      c = SLIP_ESC;
    } else {
      state = STATE_RUBBISH;
      data_len = 0;	/* remove rubbish */
      return 0;
    }
    state = STATE_OK;
    break;

  case STATE_OK:
    if(c == SLIP_ESC) {
      state = STATE_ESC;
      return 0;
    } else if(c == SLIP_END) {
      if(data_len > 0) {
        state = STATE_PROCESSING;
        process_poll(&packetizer_process);
        return 1;
      }
      return 0;
    }
    break;
  }

  /* add_char: */
  {
    rxbuf[data_len++] = c;
  }

 return 0;
}
/*---------------------------------------------------------------------------*/
int
get_packet_data_len(void)
{
  return data_len;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(packetizer_process, ev, data)
{
  PROCESS_BEGIN();
  
  packet_data_event_message = process_alloc_event();

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
    
    process_post(PROCESS_BROADCAST, packet_data_event_message, rxbuf);
    PRINTF("Data received %d\n",  get_packet_data_len());  
    /* Wait until tikiridb processes has handled the serial line event */
    if(PROCESS_ERR_OK ==
      process_post(PROCESS_CURRENT(), PROCESS_EVENT_CONTINUE, NULL)) {
      PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
    }
    data_len = 0; 
    state = STATE_OK; 
  
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
packetizer_send(const void *buffer, int len)
{
  int i;
  unsigned char c;

  packetizer_arch_writeb(SLIP_END);
  for(i=0; i<len; i++) {
    c = ((unsigned char *)buffer)[i];
    switch(c) {
      case SLIP_END:
        packetizer_arch_writeb(SLIP_ESC);
        c = SLIP_ESC_END;
        break;
      case SLIP_ESC:
        packetizer_arch_writeb(SLIP_ESC);
        c = SLIP_ESC_ESC;
        break;
    }
    packetizer_arch_writeb(c);
  }
  packetizer_arch_writeb(SLIP_END);
}
/*---------------------------------------------------------------------------*/
void 
packetizer_init(void)
{
  PRINTF("Initializing packetizer ...\n");
  data_len = 0;
  state = STATE_RUBBISH;
  packetizer_arch_init();
  process_start(&packetizer_process, NULL);
}

