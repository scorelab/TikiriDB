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
 *\file
 *      This file is part of the tikiridb system.
 *
 * \author
 *      Kasun Hewage <kch@ucsc.cmb.ac.lk>
 *      nayanajith Laxaman <nml@ucsc.cmb.ac.lk>
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "socket.h"
#include "sf-client.h"
#include "version.h"
#include "messages.h"
#include "attr-index.h"

#define DEBUG 1

#ifdef DEBUG
#define LOG_DEBUG(...) printf("[ DEBUG ] "__VA_ARGS__)
#define LOG_ERROR(...) printf("[ ERROR ] "__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_ERROR(...)
#endif

/*---------------------------------------------------------------------------*/
int
handshake(int sockfd, int mode)
{
  char buf[4]; // for initial handshaking...
  int k;

  // set the version of the sf client
  buf[0] = VERSION_MAJOR;
  buf[1] = VERSION_MINOR;
  // set connecting mode
  buf[2] = mode;
  buf[3] = 0; // not used for now.

  k = send_all(sockfd, buf, 4);
  if (k != 4)
    {
      perror("Can not send handshaking bytes to sf server");
      return ERR_UNKNOWN;
    }

  k = read_all(sockfd, buf, 4);
  if (k != 4)
    {
      perror("Can not receive handshaking bytes fron sf server");
      return ERR_UNKNOWN;
    }

  return buf[2];

}

/*---------------------------------------------------------------------------*/
int
send_packet(int sockfd, packet_t * packet)
{
  char buf[4];
  int k;

  buf[0] = packet->len & 0xFF;
  buf[1] = (packet->len >> 8) & 0xFF;
  buf[2] = packet->type;
  buf[3] = 0; // not used for now

  k = send_all(sockfd, buf, 4);
  if (k != 4)
    {
      perror("Can not send packet meta data");
      return -1;
    }

  k = send_all(sockfd, packet->data, packet->len);
  if (k != packet->len)
    {
      perror("Can not send packet data");
      return -1;
    }

  return 0;
}
/*---------------------------------------------------------------------------*/
int
receive_packet(int sockfd, packet_t * packet)
{
  char buf[4];
  int len;
  int k;

  if (packet == NULL)
    {
      return -1;
    }

  k = read_all(sockfd, buf, 4);
  if (k != 4)
    {
      perror("Can not receive packet meta data");
      return -1;
    }
  len = buf[0] + (buf[1] << 8);

  if (len > MAX_PKT_SIZE)
    {
      printf("maximum packet size exceeded. len %d\n", len);
      return -1;
    }

  k = read_all(sockfd, packet->data, len);
  if (k != len)
    {
      perror("Can not receive packet data");
      return -1;
    }
  packet->type = buf[2];
  packet->len = len;

  return 0;
}
static int
get_random(int max)
{
  return rand() % max;
}
/*---------------------------------------------------------------------------*/
void
generate_dummy_packet(packet_t * packet)
{
  srand(time(NULL));
  int len = get_random(MAX_PKT_SIZE);
  int i;

  if (packet == NULL)
    {
      return;
    }
  for (i = 0; i < len; i++)
    {
      packet->data[i] = get_random(254);
    }
  packet->len = len;

}

/*---------------------------------------------------------------------------*/
void
print_packet(packet_t * packet)
{
#define ROW_COUNT 16

  int i, k, printed, left, end;
  int nrows;

  if (packet == NULL)
    {
      return;
    }

  for (i = 0; i < 2 * ROW_COUNT; i++)
    {
      printf("=");
    }
  printf("\n");
  printf("length : %d\n", packet->len);
  printf("data   :\n");
  nrows = packet->len / ROW_COUNT;
  for (i = 0; i <= nrows; i++)
    {
      printf("------- ");
      printed = ROW_COUNT * i;
      left = packet->len - printed;
      end = printed + ((left > ROW_COUNT) ? ROW_COUNT : left);
      for (k = printed; k < end; k++)
        {
          printf("%02X ", (unsigned char) packet->data[k]);
        }
      printf("\n");
    }
  fflush(stdout);
}

/*---------------------------------------------------------------------------*/
char *
field_name(int field_id)
{
  switch (field_id)
    {
  case NODE:
    return "node";
    break;
  case TEMP:
    return "temp";
    break;
  case LIGHT:
    return "light";
    break;
  case HUMID:
    return "humid";
    break;
  case ACCELX:
    return "accelx";
    break;
  case ACCELY:
    return "accely";
    break;
  case MAGX:
    return "magx";
    break;
  case MAGY:
    return "magy";
    break;
  case ECHO:
    return "echo";
    break;
    }
  return "invalid";
}
/*---------------------------------------------------------------------------*/
#define BUFFER_SIZE 40
#define PRINT_LENGTH 30
#define FIELD_LENGTH 10
char buffer[BUFFER_SIZE]; /*Message receive buffer for the socket*/

char large_str[128];
char *
print_line(int length)
{
  bzero(large_str, 128);
  strcat(large_str, "+");
  int j;
  for (j = 0; j < length; j++)
    {
      strcat(large_str, "-");
    }
  strcat(large_str, "+");
  return large_str;
}

/*---------------------------------------------------------------------------*/
char*
fix_width(char str[])
{
  bzero(large_str, 128);
  strcat(large_str, "|");
  int i;
  for (i = 0; i < (FIELD_LENGTH - strlen(str)); i++)
    {
      strcat(large_str, " ");
    }
  strcat(large_str, str);
  return large_str;
}

/*---------------------------------------------------------------------------*/

/*printing received data from tikirdb*/
int title_printed = 0;
int node_array[128];
int node_count = 1;
void
print_rsv_packet(packet_t * packet)
{
  if (packet == NULL)
    {
      return;
    }
  /*
   printf("length : %d\n", packet->len);
   int k;
   for (k = 0; k < packet->len; k++)
   {
   printf("%d ", (unsigned char) packet->data[k]);
   }
   printf("\n");
   */

  message_header_t * message_header = (message_header_t *) packet->data;
  qresult_header_t * qresult_header = (qresult_header_t *) (message_header + 1);

  /*
   printf("epoch:%d\n", (uint16_t) ntoh_leuint16(qresult_header->epoch.data));
   printf("nrfields:%d\n", (uint8_t) qresult_header->nrfields);
   printf("qid:%d\n", (uint8_t) qresult_header->qid);
   printf("type:%d\n", (uint8_t) qresult_header->type);
   */

  char node_id[16];
  bzero(node_id, 16);
  sprintf(node_id, "%d.%d", qresult_header->nodeaddr.u8[0],
      qresult_header->nodeaddr.u8[1]);

  /*Counting the number of nodes*/
  int node_exists = 0;
  int i;
  for (i = 1; i < node_count; i++)
    {
      if (qresult_header->nodeaddr.u8[0] == node_array[i])
        {
          node_exists = 1;
        }
    }
  if (!node_exists)
    {
      node_array[node_count++] = qresult_header->nodeaddr.u8[0];
    }

  char table_header[128];
  char values[128];
  char epoch[] = "epoch";
  char tmp[18];

  strcat(table_header, fix_width(epoch));
  bzero(tmp, 18);
  sprintf(tmp, "%d", ntoh_leuint16(qresult_header->epoch.data));
  strcpy(values, fix_width(tmp));

  strcat(table_header, fix_width(field_name(1)));
  strcat(values, fix_width(node_id));

  int num_fields = (uint8_t) qresult_header->nrfields;
  rfield_t *rfield = (rfield_t *) (qresult_header + 1);
  for (i = 0; i < num_fields; i++)
    {

      strcat(table_header, fix_width(field_name((uint8_t) rfield->id)));
      bzero(tmp, 18);
      sprintf(tmp, "%d", ntoh_leuint16(rfield->data.data_bytes));
      strcat(values, fix_width(tmp));

      rfield++;
    }

  if (!title_printed)
    {
      printf("%s\n", print_line(FIELD_LENGTH * (num_fields + 2) + num_fields
          + 1));
      printf("%s|\n", table_header);
      printf("%s\n", print_line(FIELD_LENGTH * (num_fields + 2) + num_fields
          + 1));
      title_printed = 1;
    }
  printf("%s|\n", values);
  printf("%s\n", print_line(FIELD_LENGTH * (num_fields + 2) + num_fields + 1));

  fflush(stdout);

  strcpy(table_header, "");
  strcpy(values, "");

}
/*---------------------------------------------------------------------------*/
int connected = 0;
int sockfd;
/*sending the query through network*/
int
send_query(packet_t * packet, int qLength, int nepoches, char *host, int port)
{
  int k;

  if (!connected)
    {
      sockfd = connect_to_server(host, port);
      if (sockfd < 0)
        {
          perror("can not connect to the server");
          return -1;
        }

      k = handshake(sockfd, CLIENT_MODE_RW);
      if (k != ERR_OK)
        {
          printf("handshake failed: error - %d\n", k);
          return -1;
        }
      connected = 1;
    }

  k = send_packet(sockfd, packet);
  if (k < 0)
    {
      printf("error sending packet");
      return -1;
    }

  int i;
  int all_epoches = nepoches * node_count;
  while (1)
    {

      k = receive_packet(sockfd, packet);
      if (k < 0)
        {
          printf("error sending packet");
          break;
        }

      if (packet->type == PKT_TYPE_DEBUG)
        {
          //fwrite(packet->data + 1, packet->len - 1, 1, stderr);
        }
      else
        {
          i++;
          print_rsv_packet(packet);
          //print_packet(packet);
        }
      all_epoches = nepoches * (node_count - 1); /*check for current all epoches*/
      if (i > all_epoches && node_count != 1)
        {
          title_printed = 0;
          break;
        }
    }
  return 0;
}
