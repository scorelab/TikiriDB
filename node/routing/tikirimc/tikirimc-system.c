#include "tikirimc-system.h"

#include "dev/serial-line.h"
#include "dev/leds.h"
#include "lib/memb.h"
#include "lib/list.h"
#include "lib/random.h"

#include <string.h>

#define TRUE 1
#define FALSE 0

#define PRINT_ADDR(any) (any.u8[0], any.u8[1])
#define PRINT_ADDR_P(any) (any->u8[0], any->u8[1])

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static const struct packetbuf_attrlist attributes[] =
  {
    TIKIRIMC_SYSTEM_ATTRIBUTES PACKETBUF_ATTR_LAST 
  };
  

typedef struct routing_entry {
  struct routing_entry * next;
  rimeaddr_t node_addr;
  uint8_t relation;
  rimeaddr_t next_hop_addr;
  uint16_t hop_count;
  uint16_t beacon_value;
  struct ctimer ctimer;
} routing_entry; 

typedef struct seq_entry {
	struct seq_entry * next;
	rimeaddr_t node;
	uint16_t seq_no;
	struct ctimer ct;
} seq_entry;


enum {
  STATE_INIT, 
  STATE_LEAF, 
  STATE_ROOT, 
  STATE_SUB_ROOT
};

enum {
	REL_NEIGHBOUR,
	REL_CHILD,
	REL_PARENT,
	REL_DECENDANT,
	REL_ROOT,
	REL_PARENT_REFUCED,
	REL_MCAST_GROUP,
};

static const char* relation_arr[] = {"REL_NEIGHBOUR", "REL_CHILD", 
		"REL_PARENT", "REL_DECENDANT", "REL_ROOT", "REL_PARENT_REFUCED", 
		"REL_MCAST_GROUP"};

static const rimeaddr_t broadcast_addr = {{255,255}};
static rimeaddr_t mcast_groups[MAX_MULTICAST_GROUPS];
static uint8_t mcast_group_counter = 0;
static uint8_t multicast_addr_no = 0;
static uint16_t start_channel;

static struct announcement beacon;
static struct subcast_conn control_conn;

static uint8_t current_state = STATE_INIT;
static uint8_t node_cost, no_of_root_nodes;
static uint16_t current_beacon;

//static routing_entry* parent_entry = NULL;
static rimeaddr_t parent, parent_next, root_addr;

LIST(routing_table);
MEMB(routing_entry_mem, routing_entry, MAX_ROUTING_ENTRIES);

LIST(seq_no_list);
MEMB(seq_entry_mem, seq_entry, MAX_SEQ_ENTRIES);

uint8_t get_node_cost();
uint16_t get_node_state();
uint8_t get_no_of_root_nodes();
uint16_t get_node_beacon_value();
char* get_node_state_string();

char* get_state_string_from_beacon(uint16_t value);
uint8_t get_comm_cost_from_beacon(uint16_t value);
uint8_t get_cost_from_beacon(uint16_t value);
uint8_t get_state_from_beacon(uint16_t value);

void calculate_values();
void add_child_node(const rimeaddr_t *addr);
uint8_t calculate_average_comm_cost();
rimeaddr_t calculate_max_comm_cost();
void add_decendent(routing_entry new);
void refresh_decendent(routing_entry new);
void send_routing_table_to_parent();
rimeaddr_t* get_next_hop(const rimeaddr_t *dest);
int update_seq_no(const rimeaddr_t *node, int16_t no);

static void remove_routing_entry(void *n);
static void handle_parent_timeout(void *n);
static void start_network(void *n);
static void remove_seq_entry(void *n);

static void received_beacon(struct announcement *a, const rimeaddr_t *from,
          uint16_t id, uint16_t value);
static void control_msg_received(struct subcast_conn *c, const rimeaddr_t *source, 
      const rimeaddr_t *destination, const rimeaddr_t *next_hop, 
      const rimeaddr_t *last_hop, const uint8_t hops, const uint8_t ttl, 
      const uint8_t header, const int16_t original_seq_no, 
      const int16_t last_hop_seq_no);
          


/*---------------------------------------------------------------------------*/

PROCESS(beacon_process, "TikiriMC Beacon Process");
PROCESS(network_init_process, "TikiriMC Network Init Process");
PROCESS(control_process, "TikiriMC Control Process");
PROCESS(ui_process, "TikiriMC User Interface Process");

/*---------------------------------------------------------------------------*/
static void 
recv_from_subcast(struct subcast_conn *sc, const rimeaddr_t *source, 
      const rimeaddr_t *destination, const rimeaddr_t *next_hop, 
      const rimeaddr_t *last_hop, const uint8_t hops, const uint8_t ttl, 
      const uint8_t header, const int16_t original_seq_no, 
      const int16_t last_hop_seq_no)
{
	printf("Packet received from %d.%d data length = %d, hdr length = %d\n", 
      source->u8[0], source->u8[1], packetbuf_datalen(), packetbuf_hdrlen());
	if((ttl - 1) < 0) {
		printf("DROPPED TTL=%d, HOPS=%d\n", ttl, hops);
		return;
	}
	
	if((header & MASK_RT_BIT) == RT_ROOT_ONLY) {
		if(current_state != STATE_ROOT) {
			printf("DROPPED ROOT_ONLY\n");
			return;
		}
	}
	
	if(rimeaddr_cmp(source, &rimeaddr_node_addr)) {
		printf("DROPPED MY PACKET\n");
		return;
	}
	if(rimeaddr_cmp(last_hop, &rimeaddr_node_addr)) {
		printf("DROPPED PACKET FORWARDED BY ME\n");
		return;
	}
		
	
	//if(update_seq_no(source, original_seq_no) == FALSE) {
		//PRINTF("OLD original seq no of %d.%d\n", source->u8[0], source->u8[1]);
		//return;
	//} else if(update_seq_no(source, last_hop_seq_no) == FALSE) {
		//PRINTF("OLD last hop seq no of %d.%d.\n", last_hop->u8[0], 
					//last_hop->u8[1]);
		//return;
	//}
	
	struct tikirimc_system_conn *c = (struct tikirimc_system_conn *)sc;
	static uint8_t pkt_hdr = 0x00;
	
	
	
	switch(header & MASK_CT_BITS) {
		case CT_BROADCAST:
		{
			printf("CT_BROADCAST %02X\n", header);
			
			if((header & MASK_RT_BIT) == RT_ROOT_ONLY) {
				if(current_state != STATE_ROOT) {
					printf("DROPPED ROOT_ONLY\n");
					return;
				} else {
					if((header & MASK_PKT_BITS) == PKT_APPLICATION_LEVEL_UCAST) {
						if(rimeaddr_cmp(destination, &rimeaddr_node_addr)) {
							if(update_seq_no(source, original_seq_no) == TRUE) {
								c->u->recv(c, source);
							}
							return;
						} else if(rimeaddr_cmp(next_hop, &rimeaddr_node_addr) || 
								rimeaddr_cmp(next_hop, &broadcast_addr)) {
							if(ttl <= 0) {
								printf("DROPPED TTL\n");
								return;
							}
							rimeaddr_t n = *get_next_hop(destination);
							if(rimeaddr_cmp(&n, &rimeaddr_null)) {								
								PRINTF("Unicast Root TTL %d\n", ttl);
								pkt_hdr = RT_ROOT_ONLY | CT_BROADCAST | 
										PKT_APPLICATION_LEVEL_UCAST;
								subcast_fwd_unicast(sc, destination, &broadcast_addr, source, 
										hops + 1, ttl - 1, pkt_hdr, original_seq_no);
								return;
							} else {
								printf("Unicast Root TTL %d\n", ttl);
								pkt_hdr = RT_ALL_NODES | CT_UNICAST | 
										PKT_APPLICATION_LEVEL;
								subcast_fwd_unicast(sc, destination, &n, source, 
										hops + 1, ttl - 1, pkt_hdr, original_seq_no);
							}
						}
					}
				}
			} else {
				if(ttl > 0) {
				subcast_fwd_broadcast(sc, source, hops + 1, ttl - 1, header, 
						original_seq_no);
				}
				if(update_seq_no(source, original_seq_no) == TRUE) {
					c->u->recv(c, source);
				}
			}		
			
			
			break;
		}
		case CT_MULTICAST:
		{
			printf("CT_MULTICAST group %d.%d\n", destination->u8[0], 
					destination->u8[1]);
			if(current_state == STATE_INIT) {
				printf("INIT Node, Multicast packet DROPPED\n");
				return;
			}
			rimeaddr_t m = *get_next_hop(destination);			
			
			if(rimeaddr_cmp(&m, &rimeaddr_null)) {
				if(current_state != STATE_ROOT) {
					printf("Multicast packet DROPPED, No routing entry\n");
					return;
				}
			}
			
			subcast_fwd_multicast(sc, destination, source, hops + 1, ttl - 1, 
					header, original_seq_no);
					
			uint8_t i = 0;
			for(i=0; i<=mcast_group_counter; i++) {
				if(rimeaddr_cmp(destination, &mcast_groups[i])) {
					if(update_seq_no(source, original_seq_no) == TRUE) {
						c->u->recv(c,source);
					}
					break;
				}
			}		
			
			break;
		}
		case CT_SUBTREE_BCAST:
		{
			printf("CT_SUBTREE_BCAST\n");
			break;
		}
		case CT_UNICAST:
		{
			//PRINTF("CT_UNICAST\n");
			
			if(rimeaddr_cmp(destination, &rimeaddr_node_addr)) {
				if(update_seq_no(source, original_seq_no) == TRUE) {
					c->u->recv(c, source);
				}
				return;
			} else if(rimeaddr_cmp(next_hop, &rimeaddr_node_addr) || 
					rimeaddr_cmp(next_hop, &broadcast_addr)) {
				if(ttl <= 0) {
					printf("DROPPED TTL\n");
					return;
				}
				rimeaddr_t n = *get_next_hop(destination);
				if(rimeaddr_cmp(&n, &rimeaddr_null)) {
					if(current_state == STATE_ROOT) {
						printf("Unicast Root TTL %d\n", ttl);
						pkt_hdr = RT_ROOT_ONLY | CT_BROADCAST | 
								PKT_APPLICATION_LEVEL_UCAST;
						subcast_fwd_unicast(sc, destination, &broadcast_addr, source, 
								hops + 1, ttl - 1, pkt_hdr, original_seq_no);
					} else if(current_state == STATE_SUB_ROOT) {
						printf("Unicast Sub-Root TTL %d\n", ttl);
						subcast_fwd_unicast(sc, destination, &parent, source, hops + 1, 
								ttl - 1, header, original_seq_no);
					} else if(current_state == STATE_LEAF) {
						printf("Unicast Leaf TTL %d\n", ttl);
						printf("DROPPED LEAF\n");
						return;
					} else {
						printf("Unicast Init TTL %d\n", ttl);
						subcast_fwd_unicast(sc, destination, &broadcast_addr, source, 
								hops + 1, ttl - 1, header, original_seq_no);
					}						
				} else {
					subcast_fwd_unicast(sc, destination, &n, source, hops + 1, ttl - 1, 
							header, original_seq_no);
				}
			}			
			break;
		}
		default:
		{
			printf("Invalid CT Bits in routing %02X\n", header);
			printf("DROPPED INVALID\n");
			break;
		}
	}
}

static const struct subcast_callbacks routing_call = {recv_from_subcast};
/*---------------------------------------------------------------------------*/

void tikirimc_system_open(struct tikirimc_system_conn *c, uint16_t channel, 
		const struct tikirimc_system_callbacks *u)
{
	start_channel = channel;
	beacon_init();
  ui_init();
  control_init();
  subcast_open(&c->c, channel, &routing_call);
  c->u = u;
	channel_set_attributes(start_channel, attributes);
}
/*---------------------------------------------------------------------------*/
void tikirimc_system_close(struct tikirimc_system_conn *c)
{
	subcast_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_send_broadcast(struct tikirimc_system_conn *c)
{
	uint8_t pkt_hdr = RT_ALL_NODES | CT_BROADCAST | PKT_APPLICATION_LEVEL;

	subcast_send_broadcast(&c->c, MAX_TTL, pkt_hdr);
  return 1;
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_send_unicast(struct tikirimc_system_conn *c, const rimeaddr_t *dest)
{
	uint8_t pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_APPLICATION_LEVEL;
	PRINTF("Tikirimc system send unicast\n");
	rimeaddr_t n = *get_next_hop(dest);
	PRINTF("next hop %d.%d\n", n.u8[0], n.u8[1]);
	if(rimeaddr_cmp(&n, &rimeaddr_null)) {
		if(current_state == STATE_ROOT) {
			PRINTF("Root\n");
			pkt_hdr = RT_ROOT_ONLY | CT_BROADCAST | PKT_APPLICATION_LEVEL_UCAST;
			subcast_send_unicast(&c->c, dest, &broadcast_addr, MAX_TTL, pkt_hdr);
		} else if(current_state == STATE_SUB_ROOT) {
			subcast_send_unicast(&c->c, dest, &parent, MAX_TTL, pkt_hdr);
		} else if(current_state == STATE_LEAF) {
			subcast_send_unicast(&c->c, dest, &parent, MAX_TTL, pkt_hdr);
		} else {
			PRINTF("rimeaddr_null + INIT = broadcast\n");
			subcast_send_unicast(&c->c, dest, &broadcast_addr, MAX_TTL, pkt_hdr);
		}						
	} else {
		subcast_send_unicast(&c->c, dest, &n, MAX_TTL, pkt_hdr);
	}
	
  return 0;
}   
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_send_multicast(struct tikirimc_system_conn *c, 
		const rimeaddr_t *group)
{		
	routing_entry *e;
	
	for(e = list_head(routing_table); e!= NULL; e=e->next) {
		if(rimeaddr_cmp(group, &e->node_addr)) {
			//ctimer(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
			break;
		}
	}
	
	if(e == NULL) {
		return 0;
	}
	uint8_t pkt_hdr = RT_ALL_NODES | CT_MULTICAST | PKT_APPLICATION_LEVEL;
  return subcast_send_multicast(&c->c, group, MAX_TTL, pkt_hdr);
} 
/*---------------------------------------------------------------------------*/
rimeaddr_t 
tikirimc_system_create_multicast_group(struct tikirimc_system_conn *c)
{
	static rimeaddr_t mcast_addr;
	mcast_addr.u8[0] = rimeaddr_node_addr.u8[0];
	mcast_addr.u8[1] = ++multicast_addr_no;
	
	routing_entry *e;
	
	for(e = list_head(routing_table); e!= NULL; e=e->next) {
		if(rimeaddr_cmp(&mcast_addr, &e->node_addr)) {
			ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
			break;
		}
	}
	
	if(e == NULL) {
		e = memb_alloc(&routing_entry_mem);
		rimeaddr_copy(&e->node_addr, &mcast_addr);
		rimeaddr_copy(&e->next_hop_addr, &mcast_addr);
		e->hop_count = MAX_TTL;
		e->relation = REL_MCAST_GROUP;
		list_add(routing_table, e);
		ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
	} else {
		//mcast_addr tikirimc_system_create_multicast_group(c);
		return rimeaddr_null;
	}
	
	uint8_t pkt_hdr;
	pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_CREATED;
	packetbuf_copyfrom(&mcast_addr, sizeof(mcast_addr));
	if(current_state != STATE_ROOT) {
		subcast_send_unicast(&control_conn, &parent, &parent, 1, pkt_hdr);
	}
	rimeaddr_copy(&mcast_groups[mcast_group_counter++],&mcast_addr);
	return mcast_addr;
	
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_join_multicast_group(struct tikirimc_system_conn *c, 
		const rimeaddr_t *group)
{
  routing_entry *e;
	
	for(e = list_head(routing_table); e!= NULL; e=e->next) {
		if(rimeaddr_cmp(group, &e->node_addr)) {
			ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
			break;
		}
	}
	
	if(e == NULL) {
		e = memb_alloc(&routing_entry_mem);
		rimeaddr_copy(&e->node_addr, group);
		rimeaddr_copy(&e->next_hop_addr, group);
		e->hop_count = MAX_TTL;
		e->relation = REL_MCAST_GROUP;
		list_add(routing_table, e);
		ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
	} else {
		//mcast_addr tikirimc_system_create_multicast_group(c);
		return 0;
	}
	
	uint8_t pkt_hdr;
	pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_CREATED;
	packetbuf_copyfrom(group, sizeof(rimeaddr_t));
	if(current_state != STATE_ROOT) {
		return subcast_send_unicast(&control_conn, &parent, &parent, 1, pkt_hdr);
	} else {
		return 1;
	}
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_invite_to_multicast_group(struct tikirimc_system_conn *c, 
		const rimeaddr_t *group, const rimeaddr_t *node)
{
	uint8_t pkt_hdr;
	pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_INVITED;
	packetbuf_copyfrom(group, sizeof(rimeaddr_t));
	
  rimeaddr_t next = *get_next_hop(node);
  if(rimeaddr_cmp(&next, &rimeaddr_null)) {
		return subcast_send_unicast(&control_conn, node, &parent, MAX_TTL, 
				pkt_hdr);
	} else {
		return subcast_send_unicast(&control_conn, node, &next, MAX_TTL, pkt_hdr);
	}
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_leave_multicast_group(struct tikirimc_system_conn *c, 
		const rimeaddr_t *group)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
int 
tikirimc_system_remove_multicast_group(struct tikirimc_system_conn *c, 
		const rimeaddr_t *group)
{
  return 0;
}
/*---------------------------------------------------------------------------*/

static void
control_msg_received(struct subcast_conn *c, const rimeaddr_t *source, 
      const rimeaddr_t *destination, const rimeaddr_t *next_hop, 
      const rimeaddr_t *last_hop, const uint8_t hops, const uint8_t ttl, 
      const uint8_t header, const int16_t original_seq_no, 
      const int16_t last_hop_seq_no)
{
	PRINTF("Control Subcast Message Received\n Source %d.%d; Dest %d.%d; "
			"Next %d.%d; Last %d.%d; Hops %d; Header %02X; Original Seq No %d; "
			"Last Hop Seq No %d\n",
						source->u8[0], source->u8[1],
						destination->u8[0], destination->u8[1],
						next_hop->u8[0], next_hop->u8[1],
						last_hop->u8[0], last_hop->u8[1], hops, header,
						original_seq_no, last_hop_seq_no);
	
	static uint8_t pkt_hdr;
	
	if((ttl - 1) < 0) {
		return;
	}
	
	//if(update_seq_no(source, original_seq_no) == FALSE) {
		//PRINTF("OLD original seq no of %d.%d\n", source->u8[0], source->u8[1]);
		//return;
	//} else if(update_seq_no(source, last_hop_seq_no) == FALSE) {
		//PRINTF("OLD last hop seq no of %d.%d.\n", last_hop->u8[0], 
				//last_hop->u8[1]);
		//return;
	//}
	
	if((header & MASK_RT_BIT) == RT_ROOT_ONLY) {
		if(current_state != STATE_ROOT) {
			return;
		}
	} 
	
	switch(header & MASK_CT_BITS) {
		case CT_UNICAST:
		{
			PRINTF("CT_UNICAST\n");
			
			if(rimeaddr_cmp(destination, &rimeaddr_node_addr) == 0 && 
					rimeaddr_cmp(next_hop, &rimeaddr_node_addr) == 0) {
				break;
			}
			
			switch(header & MASK_PKT_BITS) {
				case PKT_PARENT_SELECT_REQ:
				{
					PRINTF("PKT_PARENT_SELECT_REQ\n");
					
					add_child_node(last_hop);
					if(current_state == STATE_LEAF) {
						current_state = STATE_SUB_ROOT;
					}
					break;
				}
				case PKT_PARENT_SELECT_RES_SUBROOT:
				{
					PRINTF("PKT_PARENT_SELECT_RES_SUBROOT\n");
					//PRINTF("#L %d 1\n", source->u8[0]);
					//if(select_parent_node(source) > 0) {
						rimeaddr_copy(&parent, source);
						rimeaddr_copy(&root_addr, (rimeaddr_t *)packetbuf_dataptr());
						printf("Parent %d.%d; Root %d.%d\n", parent.u8[0], parent.u8[1], 
								root_addr.u8[0], root_addr.u8[1]);
						current_state = STATE_SUB_ROOT;
            printf("State changed to Sub-Root\n"); 
					break;
				}
				case PKT_PARENT_SELECT_RES_LEAF:
				{
					PRINTF("PKT_PARENT_SELECT_RES_LEAF\n");
					//PRINTF("#L %d 1\n", source->u8[0]);
					//if(select_parent_node(source) > 0) {
						rimeaddr_copy(&parent, source);
						rimeaddr_copy(&root_addr, (rimeaddr_t *)packetbuf_dataptr());
						printf("Parent %d.%d; Root %d.%d\n", parent.u8[0], parent.u8[1], 
								root_addr.u8[0], root_addr.u8[1]);
						current_state = STATE_LEAF;
            printf("State changed to Leaf\n"); 
					break;
				}
				case PKT_ADD_NEW_DECENDENT:
				{
					PRINTF("PKT_ADD_NEW_DECENDENT\n");
					
					routing_entry new = *(routing_entry *)packetbuf_dataptr();
					add_decendent(new);
					
					break;
				}
				case PKT_DECENDENT_ENTRY_REFRESH:
				{
					PRINTF("PKT_DECENDENT_ENTRY_REFRESH\n");
					
					routing_entry new = *(routing_entry *)packetbuf_dataptr();
					refresh_decendent(new);
					
					break;
				}
				case PKT_MCAST_GROUP_CREATED:
				{
						PRINTF("PKT_MCAST_GROUP_CREATED %d.%d\n", 
								((rimeaddr_t *)packetbuf_dataptr())->u8[0], 
								((rimeaddr_t *)packetbuf_dataptr())->u8[1]);
								
						routing_entry *e;
	
						for(e = list_head(routing_table); e!= NULL; e=e->next) {
							if(rimeaddr_cmp(((rimeaddr_t *)packetbuf_dataptr()), 
									&e->node_addr)) {
								ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
										remove_routing_entry, e);
								break;
							}
						}
						
						if(e == NULL) {
							e = memb_alloc(&routing_entry_mem);
							rimeaddr_copy(&e->node_addr, 
									((rimeaddr_t *)packetbuf_dataptr()));
							rimeaddr_copy(&e->next_hop_addr, 
									((rimeaddr_t *)packetbuf_dataptr()));
							e->hop_count = MAX_TTL;
							e->relation = REL_MCAST_GROUP;
							list_add(routing_table, e);
							ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
									remove_routing_entry, e);
						}
						
						pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_CREATED;
						packetbuf_copyfrom((rimeaddr_t *)packetbuf_dataptr(), 
								sizeof(rimeaddr_t));
						if(current_state != STATE_ROOT) {
							subcast_send_unicast(&control_conn, &parent, &parent, 1, 
									pkt_hdr);
						}
						break;
				}
				case PKT_MCAST_GROUP_INVITED:
				{
					if(rimeaddr_cmp(destination, &rimeaddr_node_addr)) {
						PRINTF("PKT_MCAST_GROUP_CREATED %d.%d\n", 
								((rimeaddr_t *)packetbuf_dataptr())->u8[0], 
								((rimeaddr_t *)packetbuf_dataptr())->u8[1]);
								
						routing_entry *e;
	
						for(e = list_head(routing_table); e!= NULL; e=e->next) {
							if(rimeaddr_cmp(((rimeaddr_t *)packetbuf_dataptr()), 
									&e->node_addr)) {
								ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
										remove_routing_entry, e);
								break;
							}
						}
						
						if(e == NULL) {
							e = memb_alloc(&routing_entry_mem);
							rimeaddr_copy(&e->node_addr, 
									((rimeaddr_t *)packetbuf_dataptr()));
							rimeaddr_copy(&e->next_hop_addr, 
									((rimeaddr_t *)packetbuf_dataptr()));
							e->hop_count = MAX_TTL;
							e->relation = REL_MCAST_GROUP;
							list_add(routing_table, e);
							ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
									remove_routing_entry, e);
						}
						
						pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_CREATED;
						packetbuf_copyfrom((rimeaddr_t *)packetbuf_dataptr(), 
								sizeof(rimeaddr_t));
						if(current_state != STATE_ROOT) {
							subcast_send_unicast(&control_conn, &parent, &parent, 1, 
									pkt_hdr);
						}
						rimeaddr_copy(&mcast_groups[mcast_group_counter++], 
								((rimeaddr_t *)packetbuf_dataptr()));
					} else {
						rimeaddr_t n = *get_next_hop(destination);
						if(rimeaddr_cmp(&n, &rimeaddr_null)) {
							if(current_state == STATE_ROOT) {
								PRINTF("Multicast Root TTL %d\n", ttl);
								pkt_hdr = RT_ROOT_ONLY | CT_BROADCAST | 
										PKT_APPLICATION_LEVEL_UCAST;
								subcast_fwd_unicast(&control_conn, destination, 
									&broadcast_addr, source, hops + 1, ttl - 1, pkt_hdr, 
									original_seq_no);
							} else if(current_state == STATE_SUB_ROOT) {
								PRINTF("Multicast Sub-Root TTL %d\n", ttl);
								subcast_fwd_unicast(&control_conn, destination, &parent, 
									source, hops + 1, ttl - 1, header, original_seq_no);
							} else if(current_state == STATE_LEAF) {
								PRINTF("Multicast Leaf TTL %d\n", ttl);
								PRINTF("DROPPED LEAF\n");
								return;
							} else {
								PRINTF("Unicast Init TTL %d\n", ttl);
								subcast_fwd_unicast(&control_conn, destination, 
										&broadcast_addr, source, hops + 1, ttl - 1, header, 
										original_seq_no);
							}						
						} else {
							subcast_fwd_unicast(&control_conn, destination, &n, source, 
									hops + 1, ttl - 1, header, original_seq_no);
						}
					}
						
					break;
				}
				default:
				{
					PRINTF("Invalid packet PKT Bits %02X\n", (header & MASK_PKT_BITS));
					break;
				}
			}
			
			break;
		}
		case CT_MULTICAST:
		{
			PRINTF("CT_MULTICAST\n");
			break;
		}		
		case CT_BROADCAST:
		{
			PRINTF("CT_BROADCAST\n");
			
			switch(header & MASK_PKT_BITS) {
				case PKT_NULL:
				{
					PRINTF("NULL PKT received\n");
					break;
				}
				case PKT_NETWORK_INIT:
				{
					PRINTF("NETWORK INIT\n");
					
					uint8_t average = *(uint8_t *)packetbuf_dataptr();
					PRINTF("Average == %d\n", average);
					if(get_comm_cost_from_beacon(get_node_beacon_value()) >= average) {
						if(get_node_state() == STATE_INIT){
							PRINTF("Network ROOT NODE\n");
							//printf("#L %d 1\n", source->u8[0]);
							network_init();
						}
					} else {
						PRINTF("Less than average %d\n", 
								get_comm_cost_from_beacon(get_node_beacon_value()));
					}
					
					break;
				}
				case PKT_APPLICATION_LEVEL_UCAST:
				{
					if((header & MASK_RT_BIT) == RT_ROOT_ONLY) {
						if(current_state != STATE_ROOT) {
							PRINTF("DROPPED ROOT_ONLY\n");
							return;
						} else {							
							if(rimeaddr_cmp(destination, &rimeaddr_node_addr)) {
								PRINTF("PKT_MCAST_GROUP_CREATED %d.%d\n", 
								((rimeaddr_t *)packetbuf_dataptr())->u8[0], 
								((rimeaddr_t *)packetbuf_dataptr())->u8[1]);
								
								routing_entry *e;
			
								for(e = list_head(routing_table); e!= NULL; e=e->next) {
									if(rimeaddr_cmp(((rimeaddr_t *)packetbuf_dataptr()), 
											&e->node_addr)) {
										ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
												remove_routing_entry, e);
										break;
									}
								}
								
								if(e == NULL) {
									e = memb_alloc(&routing_entry_mem);
									rimeaddr_copy(&e->node_addr, 
											((rimeaddr_t *)packetbuf_dataptr()));
									rimeaddr_copy(&e->next_hop_addr, 
											((rimeaddr_t *)packetbuf_dataptr()));
									e->hop_count = MAX_TTL;
									e->relation = REL_MCAST_GROUP;
									list_add(routing_table, e);
									ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, 
											remove_routing_entry, e);
								}
								
								pkt_hdr = RT_ALL_NODES | CT_UNICAST | PKT_MCAST_GROUP_CREATED;
								packetbuf_copyfrom((rimeaddr_t *)packetbuf_dataptr(), 
										sizeof(rimeaddr_t));
								if(current_state != STATE_ROOT) {
									subcast_send_unicast(&control_conn, &parent, &parent, 1, 
											pkt_hdr);
								}
								rimeaddr_copy(&mcast_groups[mcast_group_counter++], 
										((rimeaddr_t *)packetbuf_dataptr()));
								return;
							} else if(rimeaddr_cmp(next_hop, &rimeaddr_node_addr) || 
									rimeaddr_cmp(next_hop, &broadcast_addr)) {
								if(ttl <= 0) {
									PRINTF("DROPPED TTL\n");
									return;
								}
								rimeaddr_t n = *get_next_hop(destination);
								if(rimeaddr_cmp(&n, &rimeaddr_null)) {								
									PRINTF("Unicast Root TTL %d\n", ttl);
									pkt_hdr = RT_ROOT_ONLY | CT_BROADCAST | 
											PKT_APPLICATION_LEVEL_UCAST;
									subcast_fwd_unicast(&control_conn, destination, 
											&broadcast_addr, source, hops + 1, ttl - 1, pkt_hdr, 
											original_seq_no);
									return;
								} else {
									PRINTF("Unicast Root TTL %d\n", ttl);
									pkt_hdr = RT_ALL_NODES | CT_UNICAST | 
											PKT_MCAST_GROUP_INVITED;
									subcast_fwd_unicast(&control_conn, destination, &n, source, 
											hops + 1, ttl - 1, pkt_hdr, original_seq_no);
								}
							}							
						}
					} 
					break;
				}
				default:
				{
					PRINTF("Invalid Packet PKT Bits %02X\n",  (header & MASK_PKT_BITS));
					break;
				}
			}
			
			break;
		}
		case CT_SUBTREE_BCAST:
		{
			PRINTF("CT_SUBTREE_BCAST\n");
			break;
		}
		default:
		{
			PRINTF("Invalid Packet CT type in control %02X\n",  
					(header & MASK_PKT_BITS));
			break;
		}
	}
}

static const struct subcast_callbacks control_call = {control_msg_received};
/*---------------------------------------------------------------------------*/


static void
received_beacon(struct announcement *a, const rimeaddr_t *from,
          uint16_t id, uint16_t value)
{
  static uint8_t comm_cost, cost;
  static char* state_ch;
  routing_entry* e;
  
  state_ch = get_state_string_from_beacon(value);
  comm_cost = get_comm_cost_from_beacon(value);
  cost = get_cost_from_beacon(value);
  
  PRINTF("%d.%d: Received beacon message from %d.%d, " \
            "Cost = %d, State = %s, Res_bits = %d\n",
            rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1],
            from->u8[0], from->u8[1],
            cost, state_ch, comm_cost);
            
  for(e=list_head(routing_table); e != NULL; e = e->next) {
    PRINTF("Inside list for %d.%d\n", e->node_addr.u8[0], e->node_addr.u8[1]);
    if(rimeaddr_cmp(from, &e->node_addr)) {
      rimeaddr_copy(&e->next_hop_addr, from);
      e->hop_count = 1;
      e->beacon_value = value;
      ctimer_set(&e->ctimer, ROUTING_ENTRY_TIMEOUT, remove_routing_entry, e);
      return;
    }
  }
  
  e = memb_alloc(&routing_entry_mem);
  
  if(e != NULL) {
    PRINTF("Creating entry for %d.%d\n", from->u8[0], from->u8[1]);
    rimeaddr_copy(&e->node_addr, from);
    rimeaddr_copy(&e->next_hop_addr, from);
    e->hop_count = 1;
    e->beacon_value = value;
    e->relation = REL_NEIGHBOUR;
    list_add(routing_table, e);
    ctimer_set(&e->ctimer, ROUTING_ENTRY_TIMEOUT, remove_routing_entry, e);
  }
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(beacon_process, ev, data)
{
  PROCESS_EXITHANDLER(announcement_remove(&beacon););
  
  PROCESS_BEGIN();
    
  //static uint16_t beacon_value; 
  static struct etimer et;
  
  memb_init(&routing_entry_mem);
  list_init(routing_table);
  
  memb_init(&seq_entry_mem);
  list_init(seq_no_list);
  
  subcast_open(&control_conn, 170, &control_call);
  
  random_init((((rimeaddr_node_addr.u8[0] << 5) ^ 
			(rimeaddr_node_addr.u8[0] << 3)) >> 2) % 255);
  node_cost = (uint8_t)random_rand() % MAX_RES_COST;
  calculate_values();
  announcement_register(&beacon, 128, get_node_beacon_value(), 
			received_beacon);
  //announcement_set_value(&beacon, get_node_beacon_value());
  while(1) {
		if(current_state == STATE_INIT) {
			etimer_set(&et, BEACON_MESSAGE_INTERVAL_INIT);
		} else {
			etimer_set(&et, BEACON_MESSAGE_INTERVAL_CONVERGED);
		}
		//PRINTF("Announcement\n");
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    announcement_listen(1);
    calculate_values();
    announcement_set_value(&beacon, get_node_beacon_value());
  }
  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(network_init_process, ev, data)
{
  PROCESS_EXITHANDLER();
  
  PROCESS_BEGIN();
  
  current_state = STATE_ROOT; 
  printf("State changed to Root\n"); 
  
  //routing_entry max;
  
	uint8_t avg = calculate_average_comm_cost();
	
	static uint8_t pkt_hdr;
	pkt_hdr = RT_ALL_NODES | CT_BROADCAST | PKT_NETWORK_INIT;
	
	PRINTF("PKT READY type = %02X, avg = %d\n",pkt_hdr, avg);
	packetbuf_copyfrom(&avg, sizeof(avg));
	//subcast_send(&control_conn, &broadcast_addr, &broadcast_addr, 
		//	&rimeaddr_node_addr, 0, 1, pkt_hdr, 100);
	subcast_send_single_hop_broadcast(&control_conn, pkt_hdr); 
    
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(ui_process, ev, data)
{
  PROCESS_BEGIN();
  
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == serial_line_event_message && data != NULL);
    
    PRINTF("Got input from serial line %s\n", (char *)data);
    
    if(strcmp((char *)data, "cost") == 0) {
      printf("Cost = %d\n", get_node_cost());
      continue;
    } else if(strcmp((char *)data, "state") == 0) {
      printf("State = %s\n", (char *)get_node_state_string());
      continue;
    } else if(strcmp((char *)data, "roots") == 0) {
      printf("No of Root Nodes = %d\n", get_no_of_root_nodes());
      continue;
    } else if(strcmp((char *)data, "beacon") == 0) {
      printf("Beacon value = %04X\n", current_beacon);
			continue;
    } else if(strcmp((char *)data, "calc") == 0) {
      calculate_values();
      continue;
    } else if(strcmp((char *)data, "rtable") == 0) {
      routing_entry *e;
      printf("~#~#~#~#~#~Routing Table~#~#~#~#~#~\n");
      for(e = list_head(routing_table); e != NULL; e = e->next) {
        printf("%d.%d via %d.%d, Relation = %s, hops = %d, "
						"beacon_value = %04X\n",
						e->node_addr.u8[0], e->node_addr.u8[1],
						e->next_hop_addr.u8[0], e->next_hop_addr.u8[1], 
						relation_arr[e->relation],
						//(e->relation == relation_arr[e->relation] ? 
								//relation_arr[e->relation] : "UNKNOWN"),
						e->hop_count, e->beacon_value);
      }
			continue;
    } else if(strcmp((char *)data, "initN") == 0) {
			printf("Start Network Init Process\n");
      network_init();
      continue;
    } else {
        printf("Invalid input\n");
        printf("Possible inputs are, cost, state, roots, beacon, rtable, "
						"initN\n");
				continue;
    }
  }
  
  PROCESS_END();
  
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(control_process, ev, data)
{
  PROCESS_BEGIN();
  
  static struct etimer et;
  static int8_t start_network_counter = START_NETWORK_COUNTER_VALUE;
  
  leds_init();  
  
  rimeaddr_copy(&root_addr, &rimeaddr_null);
  rimeaddr_copy(&parent, &rimeaddr_null);
  rimeaddr_copy(&parent_next, &rimeaddr_null);
  
  
  while(1) {
    etimer_set(&et, CONTROL_PROCESS_WAIT_TIME);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    calculate_values();
    announcement_set_value(&beacon, get_node_beacon_value());
    
    leds_blink();
    if(current_state == STATE_INIT) {
			start_network_counter--;
			
      leds_off(LEDS_RED+LEDS_GREEN+LEDS_BLUE);
      routing_entry *e, *max;
      
      max = NULL;
      
      for(e = list_head(routing_table); e != NULL; e = e->next) {
				if(get_state_from_beacon(e->beacon_value) != STATE_INIT){					
					if(max == NULL) {
						max = e;
					} else {
						if(get_comm_cost_from_beacon(e->beacon_value) > 
								get_comm_cost_from_beacon(max->beacon_value)) {
							max = e;
						}
					}
				}					
			}
			
			if(max != NULL) {
				uint8_t pkt_hdr = CT_UNICAST | PKT_PARENT_SELECT_REQ;				
				//subcast_send(&control_conn, &max->node_addr, &max->next_hop_addr, 
					//	&rimeaddr_node_addr, 0, 1, pkt_hdr, 0);
				subcast_send_unicast(&control_conn, &max->node_addr, 
						&max->next_hop_addr, max->hop_count + 1, pkt_hdr);
			} 
			
			if(start_network_counter < 20) {
				PRINTF("Start network counter %d\n", start_network_counter);
			}
			if(start_network_counter < 0) {
				PRINTF("Start Network\n");
				start_network_counter = START_NETWORK_COUNTER_VALUE;
				start_network(NULL);
			}
			continue;
    } else if(current_state == STATE_LEAF) {
      PRINTF("Leaf state\n");
      leds_off(LEDS_RED+LEDS_GREEN+LEDS_BLUE);
      leds_on(LEDS_GREEN);
      start_network_counter = START_NETWORK_COUNTER_VALUE;
      continue;
    } else if(current_state == STATE_ROOT) {
      PRINTF("Root state\n");
      leds_off(LEDS_RED+LEDS_GREEN+LEDS_BLUE);
      leds_on(LEDS_RED);
      start_network_counter = START_NETWORK_COUNTER_VALUE;
      rimeaddr_copy(&root_addr, &broadcast_addr);
			rimeaddr_copy(&parent, &broadcast_addr);
			rimeaddr_copy(&parent_next, &broadcast_addr);
  
      continue;
    } else if(current_state == STATE_SUB_ROOT) {
      PRINTF("SubRoot state\n");
      leds_off(LEDS_RED+LEDS_GREEN+LEDS_BLUE);
      leds_on(LEDS_BLUE);
      start_network_counter = START_NETWORK_COUNTER_VALUE;
      send_routing_table_to_parent();
      continue;
    } else {      
      PRINTF("Invalid state\n");
      continue;
    }
  }
  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

uint8_t get_node_cost()
{
  return node_cost;
}

uint8_t get_no_of_root_nodes()
{ 
  return no_of_root_nodes;
}

uint16_t get_node_beacon_value()
{  
  return current_beacon;
}

uint16_t get_node_state()
{
  return (uint16_t)current_state;
}

char* get_node_state_string()
{
  static char* array[] = {"INIT", "LEAF", "ROOT", "SUB_ROOT"};
  return array[current_state];
}

/*---------------------------------------------------------------------------*/
void calculate_values()
{ 
  static uint16_t res_cost = 0, comm_cost = 0;
  uint16_t roots = 0, beacon = 0x0000, state;
  routing_entry *e;
  
  res_cost = (uint16_t)node_cost;
  
  for(e=list_head(routing_table); e != NULL; e = e->next) {
     if(get_state_from_beacon(e->beacon_value) == (uint8_t)STATE_INIT) {
       comm_cost += COMM_COST_INIT_WEIGHT;
     } else if(get_state_from_beacon(e->beacon_value) == (uint8_t)STATE_LEAF) {
       comm_cost += COMM_COST_LEAF_WEIGHT;
     } else if(get_state_from_beacon(e->beacon_value) == (uint8_t)STATE_ROOT) {
       comm_cost += COMM_COST_ROOT_WEIGHT;
       roots++;
     } else if(get_state_from_beacon(e->beacon_value) == 
				(uint8_t)STATE_SUB_ROOT) {
       comm_cost += COMM_COST_SUB_ROOT_WEIGHT;
     }
  }
  
  if(comm_cost > MAX_COMM_COST) {
		comm_cost = MAX_COMM_COST;
	}
	if(res_cost > MAX_RES_COST) {
		res_cost = MAX_RES_COST;
	}
  
  state = (uint16_t)get_node_state();
  beacon = (state << 14) + (comm_cost << 8) + res_cost;
  PRINTF("Calculated Beacon %X, %d from state = %X, COMM_COST = %X, "
			"RES_COST = %X\n", beacon, beacon, state, comm_cost, res_cost);
  node_cost = res_cost;
  no_of_root_nodes = roots;
  current_beacon = beacon;
  comm_cost = 0;
}

/*---------------------------------------------------------------------------*/

char* get_state_string_from_beacon(uint16_t value)
{
  static char* array[] = {"INIT", "LEAF", "ROOT", "SUB_ROOT"};
  uint8_t state = (uint8_t)((value & 0xC000) >> 14);
  return array[state];
}

uint8_t get_comm_cost_from_beacon(uint16_t value)
{
  return (uint8_t)((value & 0x3F00) >> 8);
}

uint8_t get_cost_from_beacon(uint16_t value)
{
  return (uint8_t)(value & 0x00FF);
}

uint8_t get_state_from_beacon(uint16_t value)
{
	return (uint8_t)((value & 0xC000) >> 14);
}

/*---------------------------------------------------------------------------*/

void beacon_init()
{
  process_start(&beacon_process, NULL);

  return;
}

void ui_init()
{
  process_start(&ui_process, NULL);

  return;
}

void control_init()
{
  process_start(&control_process, NULL);

  return;
}

void network_init()
{
  process_start(&network_init_process, NULL);

  return;
}

/*---------------------------------------------------------------------------*/

static void
remove_routing_entry(void *n)
{
  routing_entry *e = n;
  rimeaddr_t temp;
  
  rimeaddr_copy(&temp, &e->node_addr);
  
  PRINTF("Removing routing entry for %d.%d\n", 
          e->node_addr.u8[0], e->node_addr.u8[1]);
  
  list_remove(routing_table, e);
  memb_free(&routing_entry_mem, e);
  
  if(rimeaddr_cmp(&temp, &parent)) {
		rimeaddr_copy(&parent, &rimeaddr_null);
		handle_parent_timeout(&parent);
		return;
	}
	
	if(rimeaddr_cmp(&temp, &parent_next)) {
		if(rimeaddr_cmp(&parent, &rimeaddr_null)) {
			rimeaddr_copy(&parent_next, &rimeaddr_null);
			handle_parent_timeout(&parent_next);
			return;
		}
	}
  
  if(list_length(routing_table) <= 0) {
		rimeaddr_copy(&parent, &rimeaddr_null);
		rimeaddr_copy(&parent_next, &rimeaddr_null);
		current_state = STATE_INIT;
	}
}

/*---------------------------------------------------------------------------*/

static void
remove_seq_entry(void *n)
{
  seq_entry *e = n;
  
  PRINTF("Removing seq entry %d for %d.%d\n", e->seq_no,
          e->node.u8[0], e->node.u8[1]);
  
  list_remove(seq_no_list, e);
  memb_free(&seq_entry_mem, e);
}

/*---------------------------------------------------------------------------*/
static void
handle_parent_timeout(void *n)
{  
  
  PRINTF("Parent timeout\n");
  
  routing_entry *e, *max;
      
	max = NULL;
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		if((get_state_from_beacon(e->beacon_value) == STATE_ROOT) || 
				(get_state_from_beacon(e->beacon_value) == STATE_SUB_ROOT)){					
			if(max == NULL) {
				max = e;
			} else {
				if(get_comm_cost_from_beacon(e->beacon_value) > 
						get_comm_cost_from_beacon(max->beacon_value)) {
					max = e;
				}
			}
		}					
	}
	
	if(max != NULL) {
		if(get_comm_cost_from_beacon(max->beacon_value) > 
				get_comm_cost_from_beacon(get_node_beacon_value())) {
			uint8_t pkt_hdr = CT_UNICAST | PKT_PARENT_SELECT_REQ;				
			//subcast_send(&control_conn, &max->node_addr, &max->next_hop_addr, 
					//&rimeaddr_node_addr, 0, 1, pkt_hdr, 0);
			subcast_send_unicast(&control_conn, &max->node_addr, &max->next_hop_addr, 
					max->hop_count + 1, pkt_hdr);
			PRINTF("PKT_PARENT_SELECT_REQ sent to %d.%d\n", max->node_addr.u8[0], 
					max->node_addr.u8[1]);
			rimeaddr_copy(&parent_next, &max->node_addr);
		} else {
			PRINTF("Became Root\n");
			current_state = STATE_ROOT;
		}
	} else {
		PRINTF("Became Root\n");
		current_state = STATE_ROOT;
	}
}

/*---------------------------------------------------------------------------*/

static void
start_network(void *n)
{
	uint8_t avg = calculate_average_comm_cost();
	
	PRINTF("Inside start network, avg %d\n", avg);
	if(get_comm_cost_from_beacon(get_node_beacon_value()) >= avg) {
		current_state = STATE_ROOT;
	}
}

/*---------------------------------------------------------------------------*/


void 
add_child_node(const rimeaddr_t *addr)
{
	routing_entry *e;
	
	for(e=list_head(routing_table); e != NULL; e = e->next) {
    if(rimeaddr_cmp(addr, &e->node_addr)) {
      e->relation = REL_CHILD;
      ctimer_set(&e->ctimer, ROUTING_ENTRY_TIMEOUT, remove_routing_entry, e);
      break;
    }
  }
  
  if(e == NULL){
		return;
	}
	static uint8_t pkt_hdr;
	if(get_comm_cost_from_beacon(e->beacon_value) > 
			(get_comm_cost_from_beacon(get_node_beacon_value()) / 2)) {
  	pkt_hdr = CT_UNICAST | PKT_PARENT_SELECT_RES_SUBROOT;
	} else {
		pkt_hdr = CT_UNICAST | PKT_PARENT_SELECT_RES_LEAF;
	}

	if(current_state == STATE_ROOT) {
		packetbuf_copyfrom(&rimeaddr_node_addr, sizeof(rimeaddr_t));
	} else {
		packetbuf_copyfrom(&root_addr, sizeof(rimeaddr_t));
	}
	subcast_send_unicast(&control_conn, addr, addr, e->hop_count + 1, pkt_hdr);
	printf("#L %d 1\n", addr->u8[0]);
	
	pkt_hdr = 0x00;
	pkt_hdr = CT_UNICAST | PKT_ADD_NEW_DECENDENT;
	routing_entry new;
	rimeaddr_copy(&new.node_addr, &e->node_addr);
	rimeaddr_copy(&new.next_hop_addr, &rimeaddr_node_addr);
	new.beacon_value = e->beacon_value;
	new.hop_count = e->hop_count + 1;
	new.relation = REL_CHILD;
	
	packetbuf_clear();
	packetbuf_copyfrom(&new, sizeof(routing_entry));
	subcast_send_unicast(&control_conn, &parent, &parent, new.hop_count + 1, 
			pkt_hdr);
}

/*---------------------------------------------------------------------------*/

uint8_t
calculate_average_comm_cost()
{
	routing_entry *e, *max;
  uint32_t total = 0;
  uint8_t avg = 0;   
  
	max = list_head(routing_table);
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		total += get_comm_cost_from_beacon(e->beacon_value);
		if(get_comm_cost_from_beacon(max->beacon_value) < 
				get_comm_cost_from_beacon(e->beacon_value)){
			max = e;
		}
		PRINTF("RES_BITS %X, %d\n", get_comm_cost_from_beacon(e->beacon_value), 
				get_comm_cost_from_beacon(e->beacon_value));
	}
	avg = (uint8_t)(total / list_length(routing_table));
	
	avg = avg + (get_comm_cost_from_beacon(max->beacon_value) - avg) * 7 / 8; //
	
	PRINTF("AVERAGE COMM COST %X, %d\n", avg, avg);
	
	return avg;
}

rimeaddr_t
calculate_max_comm_cost()
{
	routing_entry *e, *max; 
  
	max = list_head(routing_table);
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		if(get_comm_cost_from_beacon(max->beacon_value) < 
				get_comm_cost_from_beacon(e->beacon_value)){
			max = e;
		}
		PRINTF("RES_BITS %X, %d\n", get_comm_cost_from_beacon(e->beacon_value), 
				get_comm_cost_from_beacon(e->beacon_value));
	}
	
	return max->node_addr;
}

void
add_decendent(routing_entry new)
{
	routing_entry *e; 
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		if(rimeaddr_cmp(&e->node_addr, &new.node_addr)){
			e->hop_count = new.hop_count;
			e->relation = REL_CHILD;
			rimeaddr_copy(&e->next_hop_addr, &new.next_hop_addr);
			
			if(current_state != STATE_ROOT) {
				uint8_t pkt_hdr = 0x00;
				pkt_hdr = CT_UNICAST | PKT_ADD_NEW_DECENDENT;
				
				rimeaddr_copy(&new.next_hop_addr, &rimeaddr_node_addr);
				new.hop_count = new.hop_count + 1;
			
				packetbuf_clear();
				packetbuf_copyfrom(&new, sizeof(routing_entry));
				subcast_send_unicast(&control_conn, &parent, &parent, 
						new.hop_count + 1, pkt_hdr);
			}
			
			return;
		}		
	}
	
	if(e == NULL) {
		e = memb_alloc(&routing_entry_mem);
	}
	
	if(e != NULL) {
		e->hop_count = new.hop_count;
		e->beacon_value = new.beacon_value;
		e->relation = new.relation;
		rimeaddr_copy(&e->next_hop_addr, &new.next_hop_addr);
		rimeaddr_copy(&e->node_addr, &new.node_addr);
		list_add(routing_table, e);
		if(e->relation == REL_MCAST_GROUP) {
			ctimer_set(&e->ctimer, MULTICAST_GROUP_TIMEOUT, remove_routing_entry, e);
		} else {
			ctimer_set(&e->ctimer, ROUTING_ENTRY_TIMEOUT, remove_routing_entry, e);
		}
		if(current_state == STATE_SUB_ROOT) {
			uint8_t pkt_hdr = 0x00;
			pkt_hdr = CT_UNICAST | PKT_ADD_NEW_DECENDENT;
			
			rimeaddr_copy(&new.next_hop_addr, &rimeaddr_node_addr);
			new.hop_count = new.hop_count + 1;
		
			packetbuf_clear();
			packetbuf_copyfrom(&new, sizeof(routing_entry));
			subcast_send_unicast(&control_conn, &parent, &parent, new.hop_count + 1, 
					pkt_hdr);
		}
	}
}

void
send_routing_table_to_parent()
{
	routing_entry *e;
	routing_entry new; 
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		if(e->relation == REL_CHILD){
			new.hop_count = e->hop_count + 1;
			new.beacon_value = e->beacon_value;
			new.relation = REL_CHILD;
			rimeaddr_copy(&new.next_hop_addr, &rimeaddr_node_addr);
			rimeaddr_copy(&new.node_addr, &e->node_addr);
			
			uint8_t pkt_hdr = 0x00;
			pkt_hdr = CT_UNICAST | PKT_DECENDENT_ENTRY_REFRESH;
		
			packetbuf_clear();
			packetbuf_copyfrom(&new, sizeof(routing_entry));
			subcast_send_unicast(&control_conn, &parent, &parent, new.hop_count + 1, 
					pkt_hdr);
			
		}		
	}
}

void 
refresh_decendent(routing_entry new)
{
	routing_entry *e; 
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		if(rimeaddr_cmp(&e->node_addr, &new.node_addr)){
			e->hop_count = new.hop_count;
			e->relation = REL_CHILD;
			rimeaddr_copy(&e->next_hop_addr, &new.next_hop_addr);			
			return;
		}		
	}
	
	if(e == NULL) {
		e = memb_alloc(&routing_entry_mem);
	}
	
	if(e != NULL) {
		e->hop_count = new.hop_count;
		e->beacon_value = new.beacon_value;
		e->relation = REL_CHILD;
		rimeaddr_copy(&e->next_hop_addr, &new.next_hop_addr);
		rimeaddr_copy(&e->node_addr, &new.node_addr);
		list_add(routing_table, e);
		
		ctimer_set(&e->ctimer, ROUTING_ENTRY_TIMEOUT, remove_routing_entry, e);
	}
}

rimeaddr_t* 
get_next_hop(const rimeaddr_t *dest)
{
	routing_entry *e; 
	
	for(e = list_head(routing_table); e != NULL; e = e->next) {
		PRINTF("Routing entry processing\n");
		if(rimeaddr_cmp(&e->node_addr, dest)){		
			return &e->next_hop_addr;
		}
	}
	
	return (&rimeaddr_null);
}

int
update_seq_no(const rimeaddr_t *node, int16_t no)
{
	seq_entry *e = NULL;
	
	for(e = list_head(seq_no_list); e != NULL; e = e->next) {
		if(rimeaddr_cmp(&e->node, node)) {
			if(e->seq_no < no) {
				e->seq_no = no;
				ctimer_set(&e->ct, SEQ_ENTRY_TIMEOUT, remove_seq_entry, e);
				printf("Update seq no of %d.%d to %d old %d\n", 
						node->u8[0], node->u8[1], no, e->seq_no);
				return TRUE;
			} else {
				printf("SEQ OLD %d.%d to %d ==> %d\n", node->u8[0], node->u8[1], no, 
						e->seq_no);
				return FALSE;
			}
		}
	}
	
	if(e == NULL) {
		e = memb_alloc(&seq_entry_mem);
	}
	
	if(e != NULL) {
		rimeaddr_copy(&e->node, node);
		e->seq_no = no;
		list_add(seq_no_list, e);
		ctimer_set(&e->ct, SEQ_ENTRY_TIMEOUT, remove_seq_entry, e);
		printf("ADD seq no of %d.%d to %d\n", node->u8[0], node->u8[1], no);
		return TRUE;
	}
	
	return FALSE;
}
			
