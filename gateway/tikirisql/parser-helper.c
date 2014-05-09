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
 * \file
 *      This file is part of the tikiridb system.
 * \author
 *      nayanajith Laxaman <nml@ucsc.cmb.ac.lk>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GATEWAY 1
#include "messages.h"
#include "attr-table.h"
#include "parser-helper.h"
#include "sf-client.h"
#include "attr-index.h"

/*key commands*/
#include <signal.h>
#include <termios.h>
#include <unistd.h>

//#define DEBUG 1

#ifdef DEBUG
#define LOG_DEBUG(...) printf("[ DEBUG ] "__VA_ARGS__)
#define LOG_ERROR(...) printf("[ ERROR ] "__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#endif

/*delete this*/
void
kbd_control();

/*Linked list to hold the fields of the query*/
struct qfield
{
  field_t *field; /*Field structure from message.h*/
  struct qfield *next;
};
typedef struct qfield qfield_t;

/*Linked list to hold expressions of the query*/
struct expression_list
{
  expression_t *expression;
  struct expression_list *next;
};
typedef struct expression_list expression_list_t;

/*Linked list to keep the tables of the query*/
struct table
{
  unsigned char name[20];
  int id;
  struct table *next;
};
typedef struct table table_t;

/*Linked lists to stor fields and tables*/
qfield_t *curr_field, *head_field;
table_t *curr_table, *head_table;
expression_list_t *curr_expression, *head_expression;

/*---------------------------------------------------------------------------*/
/*storing sample period and for period*/
int sample_period = 0;
int for_period = 1000;
set_sample_period(int i)
{
  sample_period = i;
}

/*---------------------------------------------------------------------------*/
set_for_period(int i)
{
  for_period = i;
  LOG_DEBUG("for_period:%d\n",for_period);
}

/*---------------------------------------------------------------------------*/
/*convert char array to int*/
int
char_arr_to_int(unsigned char *pBytes, int size)
{
  int i = 0, arrToInt = 0;
  for (i = 0; i < size; i++)
    {
      arrToInt = (arrToInt << 8) | pBytes[i];
    }
  return arrToInt;
}

/*---------------------------------------------------------------------------*/
/*convert int to char array*/
void
int_to_char_arr(int val, unsigned char *pBytes)
{
  pBytes[1] = (unsigned char) val;
  pBytes[0] = (unsigned char) (val >> 8);
}

/*---------------------------------------------------------------------------*/
/*Adding fields of the query to a liked list eg: temp, humid*/
static int num_fields = 0;
static int curr_field_id = 0;
int
add_field(unsigned char field_name[])
{
  int field_id = get_field_id(field_name);/*set this value conditionally (2=>TEMP)*/
  int new_field = 1;/*is it a new field?*/

  /*searching whether this field is a new field*/
  qfield_t *ifield = curr_field;
  int index;
  for (index = 0; index < num_fields - 1; index++)
    {
      if (ifield->field->id == field_id)
        {
          new_field = 0;
          break;
        }
      ifield = ifield->next;
    }

  /*if it is a new field add to the field list*/
  if (new_field == 1)
    {
      curr_field = (qfield_t *) malloc(sizeof(qfield_t));
      field_t *field = (field_t *) malloc(sizeof(field_t));
      field->id = field_id;
      field->in_result = 1;
      field->type = 1;

      curr_field_id = field->id;

      curr_field->field = field;
      curr_field->next = head_field;

      head_field = curr_field;
      LOG_DEBUG("adding_fields:%d\n",num_fields);
      return num_fields++; /*return the index of newly added field*/
    }
  return index; /*return the index of already available field*/
}

/*---------------------------------------------------------------------------*/
/*Adding expressions to the linked list*/
int num_expressions = 0;
int
add_expression(int lvalue_index, int op, int rvalue)
{
  curr_expression = (expression_list_t *) malloc(sizeof(expression_list_t));
  expression_t *expression = (expression_t *) malloc(sizeof(expression_t));

  expression->l_value_index = lvalue_index;
  expression->op = op;

  hton_leuint16(&expression->r_value, rvalue);

  curr_expression->expression = expression;
  curr_expression->next = head_expression;

  head_expression = curr_expression;
  LOG_DEBUG("adding_expression:%d\n",num_expressions);
  num_expressions++;
}

/*---------------------------------------------------------------------------*/
/*Adding tables eg: sensors, buffer*/
int num_tables = 0;
int
add_table(unsigned char name[])
{
  curr_table = (table_t *) malloc(sizeof(table_t));
  sprintf(curr_table->name, "%s", name);
  curr_table->next = head_table;
  head_table = curr_table;

  LOG_DEBUG("adding_table:%d\n",num_tables);
  num_tables++;
}

/*---------------------------------------------------------------------------*/
/////////////////////where clause///////////////////////
/*Adding rvalue*/
int r_value = 0;
unsigned char * l_value;

int
add_rvalue(int rvalue)
{
  LOG_DEBUG("rvalue:%d\n", rvalue);
  r_value = rvalue;
}

/*---------------------------------------------------------------------------*/
/*Adding lvalue*/
int
add_lvalue(unsigned char lvalue[])
{
  LOG_DEBUG("lvalue:%s\n", lvalue);
  l_value = (char*) malloc(sizeof(lvalue));
  strcpy(l_value, lvalue);
}

/*---------------------------------------------------------------------------*/
/*Adding comparison operator*/
int
add_comparison(unsigned char comp[])
{
  LOG_DEBUG("comp:%s\n", comp);
  add_expression(add_field(l_value), get_operator_id(comp),
      (unsigned char) r_value);
  free(l_value);
}

/*---------------------------------------------------------------------------*/
/*Retern field id number according to the query*/
unsigned char
get_field_id(unsigned char field_name[])
{
  unsigned char field_id = 0;
  if (strcmp(field_name, "node") == 0)
    {
      field_id = NODE;
    }
  else if (strcmp(field_name, "temp") == 0)
    {
      field_id = TEMP;
    }
  else if (strcmp(field_name, "light") == 0)
    {
      field_id = LIGHT;
    }
  else if (strcmp(field_name, "humid") == 0)
    {
      field_id = HUMID;
    }
  else if (strcmp(field_name, "accelx") == 0)
    {
      field_id = ACCELX;
    }
  else if (strcmp(field_name, "accely") == 0)
    {
      field_id = ACCELY;
    }
  else if (strcmp(field_name, "magx") == 0)
    {
      field_id = MAGX;
    }
  else if (strcmp(field_name, "magy") == 0)
    {
      field_id = MAGY;
    }
  else if (strcmp(field_name, "echo") == 0)
    {
      field_id = ECHO;
    }
  return field_id;
}

/*---------------------------------------------------------------------------*/
/*Retern operator id number according to the query*/
unsigned char
get_operator_id(unsigned char operator[])
{
  //EQ = 0, NEQ = 1, GT = 2, GE = 3, LT = 4, LE = 5
  unsigned char operator_id;
  if (strcmp(operator, "=") == 0)
    {
      operator_id = EQ;
    }
  else if (strcmp(operator, "<>") == 0)
    {
      operator_id = NEQ;
    }
  else if (strcmp(operator, "<") == 0)
    {
      operator_id = LT;
    }
  else if (strcmp(operator, ">") == 0)
    {
      operator_id = GT;
    }
  else if (strcmp(operator, "<=") == 0)
    {
      operator_id = LE;
    }
  else if (strcmp(operator, ">=") == 0)
    {
      operator_id = GE;
    }
  return operator_id;
}

/*---------------------------------------------------------------------------*/
/*Filling the structured required to send the query*/

int query_id = 1;

int
send_query_to_sf(unsigned char *host, int port)
{

  packet_t packet;
  /*
   generate_simple_query(&packet); //demo
   print_packet(&packet); //demo
   */

  /*
   packet_t packets;
   generate_simple_query(&packets);//print a sample packet
   print_packet(&packets);
   */

  int pkt_size = generate_query(&packet);//fill the packet
  //print_packet(&packet);//print the packet
  send_query(&packet, pkt_size, (sample_period / for_period), host, port); /*Sending packet over the net*/
}

/*---------------------------------------------------------------------------*/
/*Query packet generator will fill the pointer given to the function and return
 * the size of the packet*/
int
generate_query(packet_t * packet)
{
  int size;
  /*Declare the variable structs*/
  /*message_header points to the data section of the packet*/
  message_header_t * message_header = (message_header_t *) packet->data;
  qmessage_header_t * qmessage_header;
  smessage_header_t * smessage_header;
  field_t * field;
  expression_t * expression;

  /*query REQUEST message*/
  message_header->type = MSG_QREQUEST;

  /*adding message header for carrying query messages*/
  qmessage_header = (qmessage_header_t *) (message_header + 1);
  qmessage_header->qid = query_id++;
  qmessage_header->qtype = 1; // this is a SELECT query
  qmessage_header->qroot.u8[0] = rimeaddr_node_addr.u8[0];
  qmessage_header->qroot.u8[1] = rimeaddr_node_addr.u8[1];

  /*adding SELECT query message header*/
  smessage_header = (smessage_header_t *) (qmessage_header + 1);
  smessage_header->nfields = num_fields;
  smessage_header->nexprs = num_expressions;
  smessage_header->in_buffer = 0;
  smessage_header->out_buffer = 0;
  /* epoch duration is 2 seconds */
  hton_leuint16(&smessage_header->epoch_duration, for_period);
  hton_leuint16(&smessage_header->nepochs, (sample_period / for_period));

  /*adding fields consists with the field id and the transformation operator.*/
  field = (field_t *) (smessage_header + 1);
  qfield_t *ifield = curr_field;
  int i;
  for (i = 0; i < num_fields; i++)
    {
      memcpy(field, ifield->field, sizeof(field_t));

      free(ifield->field);/*Free memory used to store fileds*/
      ifield = ifield->next;/*start with next item of the list*/

      field++;
    }

  /*add comparison expressions in where clause*/
  expression = (expression_t *) field;
  expression_list_t *iexpression = curr_expression;
  for (i = 0; i < num_expressions; i++)
    {

      /* index of temp in field list */
      memcpy(expression, iexpression->expression, sizeof(expression_t));

      free(iexpression->expression);/*Free memory used to store expressions*/
      iexpression = iexpression->next; /*start with next item of the list*/

      expression++;
    }

  /*calculating the size of the message*/
  size = sizeof(message_header_t) + sizeof(qmessage_header_t)
      + sizeof(smessage_header_t) + (sizeof(field_t) * num_fields)
      + (sizeof(expression_t) * num_expressions);

  /*resetting counters*/
  num_fields = 0;/*resetting field queue*/
  num_tables = 0;/*resetting table queue*/
  num_expressions = 0;/*Resetting expression queue*/

  packet->len = size; /*Setting the message size in packet*/
  return size;
}

/*---------------------------------------------------------------------------*/
/*
 *Test query packet generator to learn the process
 *
 * SELECT node, temp
 * FROM sensors
 * WHERE temp < 20
 * SAMPLE PERIOD 2 FOR 10
 */
int
generate_simple_query(packet_t * packet)
{
  int size;

  message_header_t * message_header = (message_header_t *) packet->data;
  qmessage_header_t * qmessage_header;
  smessage_header_t * smessage_header;
  field_t * field;
  expression_t * expression;

  /*
   printf("sizeof(message_header_t) %d\n", sizeof(message_header_t));
   printf("sizeof(qmessage_header_t) %d\n", sizeof(qmessage_header_t));
   printf("sizeof(smessage_header_t) %d\n", sizeof(smessage_header_t));
   printf("sizeof(field_t) %d\n", sizeof(field_t));
   printf("sizeof(expression_t) %d\n", sizeof(expression_t));
   printf("sizeof(qresult_header_t) %d\n", sizeof(qresult_header_t));
   printf("sizeof(rfield_t) %d\n", sizeof(rfield_t));
   */

  /* set message type as query request. */
  message_header->type = MSG_QREQUEST;

  /*align qmessage_header in packet data*/
  qmessage_header = (qmessage_header_t *) (message_header + 1);
  qmessage_header->qid = 1;
  qmessage_header->qtype = 1; // this is a SELECT query
  qmessage_header->qroot.u8[0] = rimeaddr_node_addr.u8[0];
  qmessage_header->qroot.u8[1] = rimeaddr_node_addr.u8[1];

  /*align smessage_header in packet data*/
  smessage_header = (smessage_header_t *) (qmessage_header + 1);
  smessage_header->nfields = 2;
  smessage_header->nexprs = 1;
  smessage_header->in_buffer = 0;
  smessage_header->out_buffer = 0;
  /* epoch duration is 2 seconds */
  hton_leuint16(&smessage_header->epoch_duration, 4);
  hton_leuint16(&smessage_header->nepochs, 5);

  /*align field in packet data*/
  field = (field_t *) (smessage_header + 1);
  field->id = TEMP; // id of node address
  field->in_result = 1;
  field->type = 3;
  field->op = 0;

  field++;

  field->id = 1; // id of temperature
  field->in_result = 1;
  field->type = 1;
  field->op = 0;

  field++;
  expression = (expression_t *) field;
  expression->l_value_index = 1; // index of temp in field list
  expression->op = EQ; // EQ, NEQ, GT, GE, LT, LE
  hton_leuint16(&expression->r_value, 20);
  LOG_DEBUG("expression->r_value:%d\n",expression->r_value);

  /*
   printf("qmessage_header_t %d, smessage_header_t %d, field_t %d, "
   "expression_t %d\n",
   sizeof(qmessage_header_t), sizeof(smessage_header_t), sizeof(field_t), sizeof(expression_t));
   */

  size = sizeof(message_header_t) + sizeof(qmessage_header_t)
      + sizeof(smessage_header_t) + (sizeof(field_t) * 2)
      + (sizeof(expression_t));

  packet->len = size;
  return size;
}

/*---------------------------------------------------------------------------*/
/*Turn terminal in to raw mode*/
struct termios oldtermios;
int
ttyraw(int fd)
{
  struct termios newtermios;
  if (tcgetattr(fd, &oldtermios) < 0)
    return (-1);
  newtermios = oldtermios;

  newtermios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* OK, why IEXTEN? If IEXTEN is on, the DISCARD character
   is recognized and is not passed to the process. This
   character causes output to be suspended until another
   DISCARD is received. The DSUSP character for job control,
   the LNEXT character that removes any special meaning of
   the following character, the REPRINT character, and some
   others are also in this category.
   */

  newtermios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* If an input character arrives with the wrong parity, then INPCK
   is checked. If this flag is set, then IGNPAR is checked
   to see if input bytes with parity errors should be ignored.
   If it shouldn't be ignored, then PARMRK determines what
   character sequence the process will actually see.

   When we turn off IXON, the start and stop characters can be read.
   */

  newtermios.c_cflag &= ~(CSIZE | PARENB);
  /* CSIZE is a mask that determines the number of bits per byte.
   PARENB enables parity checking on input and parity generation
   on output.
   */

  newtermios.c_cflag |= CS8;
  /* Set 8 bits per character. */

  newtermios.c_oflag &= ~(OPOST);
  /* This includes things like expanding tabs to spaces. */

  newtermios.c_cc[VMIN] = 1;
  newtermios.c_cc[VTIME] = 0;

  /* You tell me why TCSAFLUSH. */
  if (tcsetattr(fd, TCSAFLUSH, &newtermios) < 0)
    return (-1);
  return (0);
}

/*---------------------------------------------------------------------------*/
/*Turn terminal into default mode*/
int
ttyreset(int fd)
{
  if (tcsetattr(fd, TCSAFLUSH, &oldtermios) < 0)
    return (-1);
  return (0);
}

/*---------------------------------------------------------------------------*/
/*turn terminal into default mode when signaled*/
void
sigcatch(int sig)
{
  ttyreset(0);
  exit(0);
}

/*---------------------------------------------------------------------------*/
/*Grabbing keyboard input an process*/
void
get_key_command()
{
  int i;
  char c;
  char buffer[256];

  /* Catch the most popular signals. */
  if ((int) signal(SIGINT, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }
  if ((int) signal(SIGQUIT, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }
  if ((int) signal(SIGTERM, sigcatch) < 0)
    {
      perror("signal");
      exit(1);
    }

  /* Set raw mode on stdin. */
  if (ttyraw(0) < 0)
    {
      fprintf(stderr, "Can't go to raw mode.\n");
      exit(1);
    }
  while ((i = read(0, &c, 1)) == 1)
    {
      switch (c)
        {
      case 13: /*ENTER*/
        break;
      case 65: /*UP*/
        printf("UP\n");
        break;
      case 66: /*DOWN*/
        printf("DOWN\n");
        break;
        }

      if ((c == 13))
        {
          break;
        }else{
          printf("%d\n",c);
          //strcat(buffer,(char*)c);
          //printf("%s\n",buffer);
        }
    }

  if (ttyreset(0) < 0)
    {
      fprintf(stderr, "Cannot reset terminal!\n");
      exit(-1);
    }

  if (i < 0)
    {
      fprintf(stderr, "Read error.\n");
      exit(-1);
    }

  exit(0);
}
