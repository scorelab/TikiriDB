#ifndef __TIKIRIMC_SYSTEM_H_
#define __TIKIRIMC_SYSTEM_H_

#include <stdio.h>
#include "contiki.h"
#include "net/rime.h"
#include "net/rime/announcement.h"

#include "config.h"
#include "subcast.h"
#include "tikirimc-header.h"


#define BEACON_ID 200
#define CHANNEL 150

struct tikirimc_system_conn;

#define TIKIRIMC_SYSTEM_ATTRIBUTES SUBCAST_ATTRIBUTES

struct tikirimc_system_callbacks{
	void (*recv)(struct tikirimc_system_conn *c, const rimeaddr_t *source);
};

struct tikirimc_system_conn{
  struct subcast_conn c;
	const struct tikirimc_system_callbacks *u;
};
	
void tikirimc_system_open(struct tikirimc_system_conn *c, uint16_t channel, 
    const struct tikirimc_system_callbacks *u);

void tikirimc_system_close(struct tikirimc_system_conn *c);	

int tikirimc_system_send_broadcast(struct tikirimc_system_conn *c);

int tikirimc_system_send_unicast(struct tikirimc_system_conn *c, 
    const rimeaddr_t *dest);

int tikirimc_system_send_multicast(struct tikirimc_system_conn *c, 
    const rimeaddr_t *group);

rimeaddr_t tikirimc_system_create_multicast_group(
    struct tikirimc_system_conn *c);

int tikirimc_system_join_multicast_group(struct tikirimc_system_conn *c, 
    const rimeaddr_t *group);

int tikirimc_system_invite_to_multicast_group(struct tikirimc_system_conn *c, 
    const rimeaddr_t *group, const rimeaddr_t *node);

int tikirimc_system_leave_multicast_group(struct tikirimc_system_conn *c, 
    const rimeaddr_t *group);

int tikirimc_system_remove_multicast_group(struct tikirimc_system_conn *c, 
    const rimeaddr_t *group);

/**
 * \brief       Initialize and start Beacon Process
 * 
 *              This function initializes and starts 
 *              beacon message sending process
 * 
 */

void beacon_init();

/**
 * \brief       Initialize and start UI Process
 * 
 *              This function initializes and starts 
 *              TikiriMC User Interface process for
 *              serial line inputs.
 * 
 */

void ui_init();

/**
 * \brief       Initialize and start Control Process
 * 
 *              This function initializes and starts 
 *              TikiriMC Control process for
 *              network maintanance.
 * 
 */

void control_init();

/**
 * \brief       Initialize and start Network Init Process
 * 
 *              This function initializes and starts 
 *              TikiriMC Network Init process for
 *              network establisment.
 * 
 */

void network_init();

#endif /* __TIKIRIMC_SYSTEM_H_ */
