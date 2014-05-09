#include "routing.h"
#include "dev/serial-line.h"
#include "dev/button-sensor.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
static void
routing_recv(struct routing_conn *c, const rimeaddr_t *source)
{
  printf("APP routing message received '%s' from %d.%d\n", 
      (char *)packetbuf_dataptr(), source->u8[0], source->u8[1]);
}
static const struct routing_callbacks routing_call = {routing_recv};
static struct routing_conn routing;
/*---------------------------------------------------------------------------*/

PROCESS(test_process, "Test process");
AUTOSTART_PROCESSES(&test_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(test_process, ev, data)
{
   
  static struct etimer et;
  //static const rimeaddr_t n1 = {{1,0}};
  //static const rimeaddr_t n2 = {{8,0}};
  //static const rimeaddr_t n3 = {{19,0}};
  //static const rimeaddr_t n4 = {{25,0}};
  static const rimeaddr_t n[] = {{{1,0}},{{8,0}},{{19,0}},{{25,0}}};
int i=0;

  PROCESS_EXITHANDLER(routing_close(&routing););

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(button_sensor);
  routing_open(&routing, 130, &routing_call);
  
  while(1) {
  
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et) ||
			     (ev == sensors_event && data == &button_sensor));
    if(data != &button_sensor) {
      continue;
    }
    printf("APP Button Clicked\n");

    packetbuf_copyfrom("SRI LANKA", 10);
    routing_send_unicast(&routing, &n[i++%4]);
  }
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
