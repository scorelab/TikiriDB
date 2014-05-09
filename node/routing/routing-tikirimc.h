

#ifndef __ROUTING_TIKIRIMC_H__
#define __ROUTING_TIKIRIMC_H__

#include "tikirimc.h"

struct __routing_conn;

struct __routing_callbacks {
  void (* recv)(struct __routing_conn *c, const rimeaddr_t *source);
};

struct __routing_conn {
  struct tikirimc_conn c;
  const struct __routing_callbacks *u;
};

void __routing_open(struct __routing_conn *c, uint16_t channel,
	      const struct __routing_callbacks *u);
	      
void __routing_close(struct __routing_conn *c);

int __routing_send_unicast(struct __routing_conn *c, const rimeaddr_t *addr);

int __routing_send_broadcast(struct __routing_conn *c);

int __routing_send_multicast(struct __routing_conn *c, const rimeaddr_t *addr);

rimeaddr_t __routing_create_multicast_group(struct __routing_conn *c);

int __routing_join_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group);

int __routing_invite_to_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group, const rimeaddr_t *node);

int __routing_leave_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group);

int __routing_remove_multicast_group(struct __routing_conn *c, 
    const rimeaddr_t *group);

#endif /* __ROUTING_TIKIRIMC_H__ */
/** @} */
/** @} */
