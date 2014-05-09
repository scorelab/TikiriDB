
#ifndef __SUBCAST_H__
#define __SUBCAST_H__

#include "net/rime/broadcast.h"
#include "contiki-net.h"

#define BROADCAST {{0,0}}
//#define PACKETBUF_ATTR_PACKET_HEADER (PACKETBUF_ATTR_MAX + 1)

struct subcast_conn;

#define SUBCAST_ATTRIBUTES  { PACKETBUF_ADDR_RECEIVER, PACKETBUF_ADDRSIZE }, \
                  { PACKETBUF_ADDR_ERECEIVER, PACKETBUF_ADDRSIZE }, \
                  { PACKETBUF_ADDR_ESENDER, PACKETBUF_ADDRSIZE }, \
                  { PACKETBUF_ATTR_HOPS, PACKETBUF_ATTR_BYTE }, \
                  { PACKETBUF_ATTR_TTL, PACKETBUF_ATTR_BYTE }, \
                  { PACKETBUF_ATTR_EPACKET_TYPE, PACKETBUF_ATTR_BYTE }, \
                  { PACKETBUF_ATTR_EPACKET_ID, PACKETBUF_ATTR_BYTE * 2 }, \
                  { PACKETBUF_ATTR_PACKET_ID, PACKETBUF_ATTR_BYTE * 2 }, \
              BROADCAST_ATTRIBUTES

struct subcast_callbacks {
  void (* recv)(struct subcast_conn *c, const rimeaddr_t *source, 
      const rimeaddr_t *destination, const rimeaddr_t *next_hop, 
      const rimeaddr_t *last_hop, const uint8_t hops, const uint8_t ttl, 
      const uint8_t header, const int16_t original_seq_no, 
      const int16_t last_hop_seq_no);
};

struct subcast_conn {
  struct broadcast_conn c;
  const struct subcast_callbacks *u;
};

void subcast_open(struct subcast_conn *c, uint16_t channel, 
    const struct subcast_callbacks *u);
    
void subcast_close(struct subcast_conn *c);

int subcast_send(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const rimeaddr_t *source, const uint8_t hops, 
    const uint8_t ttl, const uint8_t header, const int16_t original_seq_no);

int subcast_send_broadcast(struct subcast_conn *c, const uint8_t ttl, 
    const uint8_t header);
    
int subcast_fwd_broadcast(struct subcast_conn *c, const rimeaddr_t *source, 
    const uint8_t hops, const uint8_t ttl, const uint8_t header, 
    const int16_t original_seq_no);
    
int subcast_send_unicast(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const uint8_t ttl, const uint8_t header);
    
int subcast_fwd_unicast(struct subcast_conn *c, const rimeaddr_t *destination, 
    const rimeaddr_t *next_hop, const rimeaddr_t *source, const uint8_t hops, 
    const uint8_t ttl, const uint8_t header, const int16_t original_seq_no);
    
int subcast_send_multicast(struct subcast_conn *c, const rimeaddr_t *group, 
    const uint8_t ttl, const uint8_t header);
    
int subcast_fwd_multicast(struct subcast_conn *c, const rimeaddr_t *group, 
    const rimeaddr_t *source, const uint8_t hops, const uint8_t ttl, 
    const uint8_t header, const int16_t original_seq_no);
    
int subcast_send_single_hop_broadcast(struct subcast_conn *c, 
    const uint8_t header);
#endif /* __SUBCAST_H__ */


