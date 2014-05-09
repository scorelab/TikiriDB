#include "socket.h"
#include "sf-client.h"

#include "messages.h"
#include "nw-types.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int generate_simple_query(packet_t * packet);

int 
main(int argc, char** argv)
{

  int sockfd; 
  int k;
  packet_t packet;

  if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
    perror("Warning: failed to ignore SIGPIPE");
  }

  sockfd = connect_to_server(argv[1], atoi(argv[2]));

  if(sockfd < 0) {
    perror("can not connect to the server");
    return -1; 
  }

  k = handshake(sockfd, CLIENT_MODE_RW);
  if(k != ERR_OK) {
    printf("handshake failed: error - %d\n", k);
    return -1;
  }

  generate_simple_query(&packet);

  k = send_packet(sockfd, &packet);
  if(k < 0) {
    printf("error sending packet");
    return -1;
  }


  while(1) {
    k = receive_packet(sockfd, &packet);
    if(k < 0) {
      printf("error sending packet");
      break;
    }
    
    if(packet.type == PKT_TYPE_DEBUG) {
      fwrite(packet.data, 1, packet.len, stdout);
    } else {
      print_packet(&packet);
    }
   

  }

 
  return 0;
}
/*---------------------------------------------------------------------------*/
/*
 * SELECT node_addr, temp
 * FROM sensors
 * WHERE temp < 20
 * SAMPLE PERIOD 2s
 */
int 
generate_simple_query(packet_t * packet)
{
  int size;
  
  message_header_t * message_header = (message_header_t *)packet->data;
  qmessage_header_t * qmessage_header = (qmessage_header_t *)(message_header + 1);
  smessage_header_t * smessage_header;
  field_t * field;
  expression_t * expression;
 
  /*
   * ************ Message header ************************************
   */ 
  /* Set message type as query request. 
   * There are two types of messages: Query Requests and Query Replys
   */
  message_header->type = MSG_QREQUEST;

  /*
   * ************ Query Request header ******************************
   */ 
  /*
   * Set Query ID.
   */
  qmessage_header->qid = 1;
  /*
   * Set Query Request type.
   */
  qmessage_header->qtype = 1; // this is a SELECT query
  
  /* Set the query root's address
   * Note: gateway node also set this before broadcasting the query
   */
  qmessage_header->qroot.u8[0] = rimeaddr_node_addr.u8[0];   
  qmessage_header->qroot.u8[1] = rimeaddr_node_addr.u8[1];  

  /*
   * ************ SELECT query header *******************************
   */ 
  smessage_header = (smessage_header_t *)(qmessage_header + 1);
  /* Total number of fields which are needed to execute the query. 
   * The fields do not need to be essentially included in the SELECT caluse; 
   * the fields included in the clauses such as WHERE are also counted.
   */ 
  smessage_header->nfields = 2;
  /*
   * Number of expressions in the query.
   */
  smessage_header->nexprs = 1;
  /*
   * This specifies from where the data should be collected in order to 
   * execute the query. This can be directly from sensors or storage buffer.
   * 0 is the default. It means, the data should be collected from the sensors. 
   * The values greater than 0 indicates the ID of the storage buffer.  
   */
  smessage_header->in_buffer = 0;
  /*
   * This specifies the result output buffer. 0 is the default which is the 
   * results of query have to be passed to the query node on the fly. The values
   * greater then 0 indicates the ID of the storage buffer.  
   */
  smessage_header->out_buffer = 0;
  /* 
   * Set epoch duration. 
   */
  hton_leuint16(&smessage_header->epoch_duration, 2);
  /*
   * Set number of epoches.
   */
  hton_leuint16(&smessage_header->nepochs, 10);

  /*
   * ************ SELECT query fields *******************************
   */ 
  field = (field_t *)(smessage_header + 1);
  /*
   * Set the global index of the attribute field. 
   * I.E. For node-address this could be 1 while for temperature it is 2.
   */
  field->id = 1;
  /*
   * Indicates whether the field should be included in the result.
   * Possible values are 1 and 0.
   */
  field->in_result = 1;
  //field->type = 3; // Not used for now
  field->op = 0;

  field++;

  field->id = 3; // id of temperature
  field->in_result = 1;
  //field->type = 1; // Not used for now
  field->op = 0;

  field++;
  
  /*
   * ************ SELECT query expressions *******************************
   * 
   * Expressions are in the following form.
   *        <l_value_index> <operator> <r_value>
   * 
   * The l_value_index is index of temp in field list. 
   * The operator is a comparisson operator such as EQ, NEQ, GT, GE, LT, LE
   * The value is what you are going to compare with the operator.
   * Assume the caluse WHERE temp > 27
   * The expresstion is 1 GT 27
   */   
  expression = (expression_t *)field;
  expression->l_value_index = 1; // index of temp in field list
  expression->op = GT; // EQ, NEQ, GT, GE, LT, LE
  hton_leuint16(&expression->r_value, 27);


  size = sizeof(message_header_t) + 
         sizeof(qmessage_header_t) + 
         sizeof(smessage_header_t) + 
         (sizeof(field_t) * smessage_header->nfields) + 
         (sizeof(expression_t) * smessage_header->nexprs ) ;

  packet->len = size;
  
  return size;
}

