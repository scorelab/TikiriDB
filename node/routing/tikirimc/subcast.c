#include "contiki.h"
#include "net/rime.h"
#include "subcast.h"
#include "random.h"
#include <string.h>
#include "rime.h"

static const struct packetbuf_attrlist attributes[] =
  {
    SUBCAST_ATTRIBUTES
    PACKETBUF_ATTR_LAST
  };

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SELF_ORIGINATED -999

static int16_t seq_no = 1;
static const rimeaddr_t broadcast_addr = {{255,255}};

/*---------------------------------------------------------------------------*/
static void
recv_from_broadcast(struct broadcast_conn *broadcast, 
    const rimeaddr_t *last_hop)
{
  rimeaddr_t *next_hop = NULL, *src = NULL, *dest = NULL;
  uint8_t ttl, hops;
  
  struct subcast_conn *c = (struct subcast_conn *)broadcast;

  PRINTF("%d.%d: uc: recv_from_broadcast, receiver %d.%d\n",
	 rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],
	 packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[0],
	 packetbuf_addr(PACKETBUF_ADDR_RECEIVER)->u8[1]);
  
  c->u->recv(c, packetbuf_addr(PACKETBUF_ADDR_ESENDER), 
      packetbuf_addr(PACKETBUF_ADDR_ERECEIVER), 
      packetbuf_addr(PACKETBUF_ADDR_RECEIVER), last_hop, 
      packetbuf_attr(PACKETBUF_ATTR_HOPS), packetbuf_attr(PACKETBUF_ATTR_TTL), 
      packetbuf_attr(PACKETBUF_ATTR_EPACKET_TYPE), 
      packetbuf_attr(PACKETBUF_ATTR_EPACKET_ID), 
      packetbuf_attr(PACKETBUF_ATTR_PACKET_ID));
  
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks uc = {recv_from_broadcast};
/*---------------------------------------------------------------------------*/
void
subcast_open(struct subcast_conn *c, uint16_t channel,
	     const struct subcast_callbacks *u)
{
  broadcast_open(&c->c, channel, &uc);
  c->u = u;
  channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void
subcast_close(struct subcast_conn *c)
{
  broadcast_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int 
subcast_send(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const rimeaddr_t *source, const uint8_t hops, 
    const uint8_t ttl, const uint8_t header, const int16_t original_seq_no)
{
  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, next_hop);
  packetbuf_set_addr(PACKETBUF_ADDR_ERECEIVER, destination);
  packetbuf_set_addr(PACKETBUF_ADDR_ESENDER, source);
  packetbuf_set_attr(PACKETBUF_ATTR_HOPS, hops);
  packetbuf_set_attr(PACKETBUF_ATTR_TTL, ttl);
  packetbuf_set_attr(PACKETBUF_ATTR_EPACKET_TYPE, header);
  
  if(original_seq_no == SELF_ORIGINATED) {
    packetbuf_set_attr(PACKETBUF_ATTR_EPACKET_ID, seq_no);
  } else {
    packetbuf_set_attr(PACKETBUF_ATTR_EPACKET_ID, original_seq_no);
  }
  
  packetbuf_set_attr(PACKETBUF_ATTR_PACKET_ID, seq_no++);
  return broadcast_send(&c->c);
  //static struct etimer et;
  //static uint8_t count = 1;
  //static uint8_t ret_val;
  
  //while(count < 7) {
    //ret_val = broadcast_send(&c->c);
    //if(ret_val == RIME_OK) {
        //printf("Success in delivery, retval == %d\n", ret_val);
        //return RIME_OK;
    //} else {
      //printf("Error in delivery, retval == %d, count == %d\n", ret_val, count);
      //etimer_set(&et, (count * count) + random_rand() % (CLOCK_SECOND * count * count));
      //count++;
      //return RIME_ERR;
    //}
 // }
  
  //return RIME_ERR;

}
/*---------------------------------------------------------------------------*/
int 
subcast_send_broadcast(struct subcast_conn *c, const uint8_t ttl, 
    const uint8_t header)
{
  return subcast_send(c, &broadcast_addr, &broadcast_addr, &rimeaddr_node_addr,
      0, ttl, header, SELF_ORIGINATED);
}
/*---------------------------------------------------------------------------*/
int 
subcast_fwd_broadcast(struct subcast_conn *c, const rimeaddr_t *source, 
    const uint8_t hops, const uint8_t ttl, const uint8_t header, 
    const int16_t original_seq_no)
{
  return subcast_send(c, &broadcast_addr, &broadcast_addr, source, hops, ttl, 
      header, original_seq_no);
}
/*---------------------------------------------------------------------------*/
int 
subcast_send_unicast(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const uint8_t ttl, const uint8_t header)
{
  return subcast_send(c, destination, next_hop, &rimeaddr_node_addr, 0, ttl, 
      header, SELF_ORIGINATED);
}
/*---------------------------------------------------------------------------*/
int 
subcast_fwd_unicast(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const rimeaddr_t *source, const uint8_t hops, 
    const uint8_t ttl, const uint8_t header, const int16_t original_seq_no)
{
  return subcast_send(c, destination, next_hop, source, hops, ttl, header, 
      original_seq_no);
}
/*---------------------------------------------------------------------------*/
int 
subcast_send_multicast(struct subcast_conn *c, const rimeaddr_t *group, 
    const uint8_t ttl, const uint8_t header)
{
  return subcast_send(c, group, group, &rimeaddr_node_addr, 0, ttl, header, 
      SELF_ORIGINATED);
}
/*---------------------------------------------------------------------------*/
int 
subcast_fwd_multicast(struct subcast_conn *c, const rimeaddr_t *group, 
    const rimeaddr_t *source, const uint8_t hops, const uint8_t ttl, 
    const uint8_t header, const int16_t original_seq_no)
{
  return subcast_send(c, group, group, source, hops, ttl, header, 
      original_seq_no);
}
/*---------------------------------------------------------------------------*/
int 
subcast_send_single_hop_broadcast(struct subcast_conn *c, 
    const uint8_t header)
{
  return subcast_send(c, &broadcast_addr, &broadcast_addr, &rimeaddr_node_addr,
      0, 1, header, SELF_ORIGINATED);
}
/*---------------------------------------------------------------------------*/
