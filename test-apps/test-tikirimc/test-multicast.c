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
  static const rimeaddr_t n1 = {{1,0}};
  static const rimeaddr_t n2 = {{8,0}};
  static const rimeaddr_t n3 = {{19,0}};
  static const rimeaddr_t n4 = {{25,0}};
  
  static rimeaddr_t mcast;
  static uint8_t cnt = 0;
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
    cnt++;
    if(cnt == 1) {
      mcast = routing_create_multicast_group(&routing);
      printf("Multicast group created %d.%d\n", mcast.u8[0], mcast.u8[1]);
    } else if(cnt == 2) {
      routing_invite_to_multicast_group(&routing, &mcast, &n1);
    } else if(cnt == 3) {
      routing_invite_to_multicast_group(&routing, &mcast, &n2);
    } else if(cnt == 4) {
      routing_invite_to_multicast_group(&routing, &mcast, &n3);
    } else if(cnt == 5) {
      routing_invite_to_multicast_group(&routing, &mcast, &n4);
    } else if(cnt > 5) {
      packetbuf_copyfrom("SRI LANKA", 10);
      routing_send_multicast(&routing, &mcast);
    }
  }
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
