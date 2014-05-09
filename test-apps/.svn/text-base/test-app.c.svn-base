#include "contiki.h"
#include "attr-table.h"
#include "tikiridb.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(startup_process, "Statup Process");

AUTOSTART_PROCESSES(&startup_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(startup_process, ev, data)
{
  PROCESS_BEGIN();
 
  tikiridb_init();
  printf("TikiriDB started.\n");

  PROCESS_END();
}

