
#include "tikirimc.h"

static const struct packetbuf_attrlist attributes[] =
  {
    TIKIRIMC_ATTRIBUTES PACKETBUF_ATTR_LAST 
  };
  
static uint16_t start_channel;
/*---------------------------------------------------------------------------*/
static void
recv_from_tikirimc_system(struct tikirimc_system_conn *sc, const rimeaddr_t *source)
{
  struct tikirimc_conn *c = (struct tikirimc_conn *)sc;
  c->u->recv(c, source);
}

static const struct tikirimc_system_callbacks su = {recv_from_tikirimc_system};
/*---------------------------------------------------------------------------*/
void 
tikirimc_open(struct tikirimc_conn *c, uint16_t channel, 
    const struct tikirimc_callbacks *u)
{
	start_channel = channel;
  tikirimc_system_open(&c->c, start_channel, &su);
  c->u = u;
	channel_set_attributes(start_channel, attributes);
}
/*---------------------------------------------------------------------------*/
void 
tikirimc_close(struct tikirimc_conn *c)
{
  tikirimc_system_close(&c->c);
}		
/*---------------------------------------------------------------------------*/
int 
tikirimc_send_broadcast(struct tikirimc_conn *c)
{
  return tikirimc_system_send_broadcast(&c->c);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_send_unicast(struct tikirimc_conn *c, const rimeaddr_t *dest)
{
  return tikirimc_system_send_unicast(&c->c, dest);
}   
/*---------------------------------------------------------------------------*/
int 
tikirimc_send_multicast(struct tikirimc_conn *c, const rimeaddr_t *group)
{
  return tikirimc_system_send_multicast(&c->c, group);
} 
/*---------------------------------------------------------------------------*/
rimeaddr_t 
tikirimc_create_multicast_group(struct tikirimc_conn *c)
{
  return tikirimc_system_create_multicast_group(&c->c);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_join_multicast_group(struct tikirimc_conn *c, const rimeaddr_t *group)
{
  return tikirimc_system_join_multicast_group(&c->c, group);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_invite_to_multicast_group(struct tikirimc_conn *c, const rimeaddr_t *group, 
    const rimeaddr_t *node)
{
  return tikirimc_system_invite_to_multicast_group(&c->c, group, node);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_leave_multicast_group(struct tikirimc_conn *c, const rimeaddr_t *group)
{
  return tikirimc_system_leave_multicast_group(&c->c, group);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_remove_multicast_group(struct tikirimc_conn *c, const rimeaddr_t *group)
{
  return tikirimc_system_remove_multicast_group(&c->c, group);
}
/*---------------------------------------------------------------------------*/
