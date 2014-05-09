#include "contiki.h"

#include "routing.h"
#include "tikiridb.h"

#include "dev/button-sensor.h"

#include <stdio.h> /* For printf() */
/*---------------------------------------------------------------------------*/
PROCESS(startup_process, "Statup Process");
PROCESS(test_process, "Test Process");
AUTOSTART_PROCESSES(&startup_process);
/*---------------------------------------------------------------------------*/
/*
 * SELECT node_addr, temp
 * FROM sensors
 * WHERE temp = 20
 * SAMPLE PERIOD 2s
 * FOR 10
 */

int 
generate_sample_squery(void * data_ptr)
{
  message_header_t * message_header = (message_header_t *)data_ptr;
  qmessage_header_t * qmessage_header = (qmessage_header_t *)(message_header + 1);
  smessage_header_t * smessage_header;
  field_t * field;
  expression_t * expression;

  /* set message type as query request. */
  message_header->type = MSG_QREQUEST;

  qmessage_header->qid = 1;
  qmessage_header->qtype = 1; // this is a SELECT query
  qmessage_header->qroot.u8[0] = rimeaddr_node_addr.u8[0];   
  qmessage_header->qroot.u8[1] = rimeaddr_node_addr.u8[1];  

  smessage_header = (smessage_header_t *)(qmessage_header + 1);
  smessage_header->nfields = 2;
  smessage_header->nexprs = 1;
  smessage_header->in_buffer = 0;
  smessage_header->out_buffer = 0;
  /* epoch duration is 2 seconds */
  hton_leuint16(&smessage_header->epoch_duration, 2);
  hton_leuint16(&smessage_header->nepochs, 5);


  field = (field_t *)(smessage_header + 1);
  field->id = 1; // id of node address
  field->in_result = 1;
  field->type = 3; 
  field->op = 0;

  field++;

  field->id = 2; // id of temperature
  field->in_result = 1;
  field->type = 1; 
  field->op = 0;

  field++;
  expression = (expression_t *)field;
  expression->l_value_index = 1; // index of temp in field list
  expression->op = EQ; // EQ, NEQ, GT, GE, LT, LE
  hton_leuint16(&expression->r_value, 20);

  printf("qmessage_header_t %d, smessage_header_t %d, field_t %d, "
         "expression_t %d\n",
         sizeof(qmessage_header_t), sizeof(smessage_header_t), sizeof(field_t), sizeof(expression_t));
  
  return sizeof(message_header_t) + sizeof(qmessage_header_t) + 
         sizeof(smessage_header_t) + (sizeof(field_t) * 2) + (sizeof(expression_t)) ;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(startup_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Startup process..\n");

  // Activate the button sensor as in the new sensor API.
  button_sensor.configure(SENSORS_ACTIVE, (void *) 1); 
 
  tikiridb_init();
  process_start(&test_process, NULL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_process, ev, data)
{
  PROCESS_BEGIN();

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);
    rimeaddr_t addr;
    int data_len;
    addr.u8[0] = 2;
    addr.u8[1] = 0;
    packetbuf_clear();
    data_len = generate_sample_squery(packetbuf_dataptr());
    printf("sizeof query %d\n", data_len);
    packetbuf_set_datalen(data_len);
    qprocessor_send_data(&addr);
    printf("message sent\n");
  }

  PROCESS_END();
}




