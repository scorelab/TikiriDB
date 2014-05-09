/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 * This file is part of the Contiki operating system.
 *
 * $Id: example-routing.c,v 1.7 2009/06/09 19:31:55 fros4943 Exp $
 */

/**
 * \file
 *         Testing the tikirimc layer in Rime
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "routing.h"
#include "contiki.h"
#include "contiki-net.h"
#include "random.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"


#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(example_routing_process, "routing example");
AUTOSTART_PROCESSES(&example_routing_process);
/*---------------------------------------------------------------------------*/
static void
recv(struct routing_conn *c, const rimeaddr_t *from, void *data)
{	
  printf("%d.%d: TikiriMC message received '%s' from %d.%d\n",
  	rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],		
  	(char *)data, from->u8[0], from->u8[1]);
}
static const struct routing_callbacks routing_call = {recv};
static struct routing_conn routing;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_routing_process, ev, data)
{
  static rimeaddr_t addr1 = {{1,0}};

  PROCESS_EXITHANDLER(routing_close(&routing);)

  PROCESS_BEGIN();

  routing_open(&routing, 128, &routing_call);
  
  SENSORS_ACTIVATE(button_sensor);

  while(1) {

   /* Delay 2-4 seconds */
   PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
			     data == &button_sensor);  

   packetbuf_copyfrom("Hello", 6);
   routing_send(&routing, &addr1);
   printf("tikirimc message sent\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
