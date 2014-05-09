#ifndef __PACKETIZER_H__
#define __PACKETIZER_H__
 
#include "contiki.h"

/* serial packet types */
#define PKT_TYPE_DEBUG 0x01
#define PKT_TYPE_RAW 0x02

extern process_event_t packet_data_event_message;

int packetizer_input_byte(unsigned char c);

int get_packet_data_len(void);

void packetizer_init(void);

void packetizer_arch_init(void);

void packetizer_arch_writeb(unsigned char c);

void packetizer_send(const void *buffer, int len);

PROCESS_NAME(packetizer_process);

#endif /* __PACKETIZER_H__ */
