#include "routing.h"
#include "dev/serial-line.h"
#include "dev/button-sensor.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/

static const struct routing_callbacks routing_call = {NULL};
static struct routing_conn routing;
/*---------------------------------------------------------------------------*/

PROCESS(test_process, "Test process");
AUTOSTART_PROCESSES(&test_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(test_process, ev, data)
{
   
  static struct etimer et;

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
  }
  

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
