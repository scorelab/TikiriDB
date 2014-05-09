
#include "routing-tikirimc.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static int
tikirimc_recv(struct tikirimc_conn *c, const rimeaddr_t *source)
{
  struct __routing_conn *r = (struct __routing_conn *)c;
  
  PRINTF("%d.%d: bc: tikirimc_recv, receiver %d.%d\n",
	 rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],
	 from->u8[0],
	 from->u8[1]);
  if(r->u->recv) {
    r->u->recv(r, source);
  }
}

static const struct tikirimc_callbacks tu = {tikirimc_recv};

void 
__routing_open(struct __routing_conn *c, uint16_t channel,
	      const struct __routing_callbacks *u)
{
  tikirimc_open(&c->c, channel, &tu);
  c->u = u;
}
	      
void 
__routing_close(struct __routing_conn *c)
{
  tikirimc_close(&c->c);
}

int 
__routing_send_unicast(struct __routing_conn *c, const rimeaddr_t *addr)
{
  tikirimc_send_unicast(&c->c, addr);
}

int 
__routing_send_broadcast(struct __routing_conn *c)
{
  tikirimc_send_broadcast(&c->c);
}

int 
__routing_send_multicast(struct __routing_conn *c, const rimeaddr_t *addr)
{
  tikirimc_send_multicast(&c->c, addr);
}

rimeaddr_t 
__routing_create_multicast_group(struct __routing_conn *c)
{
  return tikirimc_create_multicast_group(&c->c);
}

int 
__routing_join_multicast_group(struct __routing_conn *c, const rimeaddr_t *group)
{
  return tikirimc_join_multicast_group(&c->c, group);
}

int 
__routing_invite_to_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group, const rimeaddr_t *node)
{
  return tikirimc_invite_to_multicast_group(&c->c, group, node);
}

int 
__routing_leave_multicast_group(struct __routing_conn *c, const rimeaddr_t *group)
{
  return tikirimc_leave_multicast_group(&c->c, group);
}

int 
__routing_remove_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group)
{
  return tikirimc_remove_multicast_group(&c->c, group);
}
