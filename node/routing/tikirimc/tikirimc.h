/**
 * \defgroup tikirimc TikiriMC - A Multicasting Enabled Routing Protocol
 * @{
 *
 * The tikirimc module is capable of sending unicast, brodcast and 
 * multicast messages.
 *
 * \section channels Channels
 *
 * The tikirimc module uses 3 channels.
 *
 */

/*
 * Copyright (c) 2009, Wireless Ad-Hoc Sensor Network Laboratory 
 * University of Colombo School of Computting.
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
 *
 * $Id: tikirimc.h,v 0.0 2010/01/18 Exp $
 */

/**
 * \file
 *         Header file for the Tikiri Multicasting Protocol (TikiriMC)
 * \author
 *         Tharindu Nanayakkara <tharindu.ucsc@gmail.com>
 */

#ifndef __TIKIRIMC_H__
#define __TIKIRIMC_H__

#include "tikirimc-system.h"

struct tikirimc_conn;

#define TIKIRIMC_ATTRIBUTES TIKIRIMC_SYSTEM_ATTRIBUTES
                        
void tikirimc_init();

/**
 * \brief	Callback structure for TikiriMC
 * 
 */
struct tikirimc_callbacks{
	/** Called when a packet have been recieved by TikiriMC module */
	void (*recv)(struct tikirimc_conn *c, const rimeaddr_t *source);
};

struct tikirimc_conn{
  struct tikirimc_system_conn c;
	const struct tikirimc_callbacks *u;
};
	
/**
 * \brief	Set up a TikiriMC connection
 * \param c	A pointer to a struct tikirimc_conn
 * \param channel	The starting channel of the connection
 * \param u	A pointor to a tikirimc_callbacks structure
 * 
 * 			This function sets up a tikirimc_conn on the specified channel.
 * 
 */
void tikirimc_open(struct tikirimc_conn *c, uint16_t channel,
		const struct tikirimc_callbacks *u);

/**
 * \brief	Close a TikiriMC connection
 * \param c	A pointor to the struct tikirimc_conn which has to be closed
 * 
 * 			This function closes the specified tikirimc_conn.
 * 
 */
 void tikirimc_close(struct tikirimc_conn *c);				

/**
 * \brief	Send a TikiriMC broadcast packet
 * \param c	The TikiriMC connection
 * \retval	Non-zero if the packet could be sent. Zero otherwise.
 * 			
 * 			This function sends a TikiriMC Broadcast packet.
 * 
 */
int tikirimc_send_broadcast(struct tikirimc_conn *c); 

/**
 * \brief	Send a TikiriMC unicast packet
 * \param c	The TikiriMC connection
 * \param dest	Address of the receiver
 * \retval	Non-zero if the packet could be sent. Zero otherwise.
 * 			
 * 			This function sends a TikiriMC Unicast packet.
 * 
 */
int tikirimc_send_unicast(struct tikirimc_conn *c, const rimeaddr_t *dest);   

/**
 * \brief	Send a TikiriMC multicast packet
 * \param c	The TikiriMC connection
 * \param group	Address of the multicast group
 * \retval	Non-zero if the packet could be sent. Zero otherwise.
 * 			
 * 			This function sends a TikiriMC Multicast packet.
 * 
 */
int tikirimc_send_multicast(struct tikirimc_conn *c, const rimeaddr_t *group); 

/**
 * \brief	Create a TikiriMC multicast group
 * \param c	The TikiriMC connection
 * \retval	Address of the multicast group if successfullly created, 
 *          rimeaddr_null otherwise.
 * 			
 * 			This function creates a TikiriMC Multicast group and returns it 
 *      address.
 * 
 */
rimeaddr_t tikirimc_create_multicast_group(struct tikirimc_conn *c);

/**
 * \brief	Join to a TikiriMC multicast group
 * \param c	The TikiriMC connection
 * \param group	Address of the multicast group
 * \retval	Non-zero if successfully joined. Zero otherwise.
 * 			
 * 			This function joins the calling node to the specified TikiriMC 
 *      Multicast group.
 * 
 */
int tikirimc_join_multicast_group(struct tikirimc_conn *c, 
    const rimeaddr_t *group);

/**
 * \brief	Invite a node to join a TikiriMC multicast group
 * \param c	The TikiriMC connection
 * \param group	Address of the multicast group
 * \param node	Address of the invited node
 * \retval	Non-zero if successfully joined. Zero otherwise.
 * 			
 * 			This function invites the specified node to join the specified 
 *      TikiriMC Multicast group.
 * 
 */
int tikirimc_invite_to_multicast_group(struct tikirimc_conn *c, 
    const rimeaddr_t *group, const rimeaddr_t *node);

/**
 * \brief	Leave from a TikiriMC multicast group
 * \param c	The TikiriMC connection
 * \param group	Address of the multicast group
 * \retval	Non-zero if successfully left. Zero otherwise.
 * 			
 * 			This function removes the calling node from the specified 
 *      TikiriMC Multicast group.
 * 
 */
int tikirimc_leave_multicast_group(struct tikirimc_conn *c, 
    const rimeaddr_t *group);

/**
 * \brief	Remove a TikiriMC multicast group
 * \param c	The TikiriMC connection
 * \param group	Address of the multicast group
 * \retval	Non-zero if successfully removed. Zero otherwise.
 * 			
 * 			This function removes the specified TikiriMC Multicast group from the 
 *      network.
 * 
 */
int tikirimc_remove_multicast_group(struct tikirimc_conn *c, 
    const rimeaddr_t *group);

#endif /* __TIKIRIMC_H__ */
/** @} */
