#include "packetizer.h"
#include "dev/rs232.h"


void 
packetizer_arch_init(void)
{
  rs232_set_input(RS232_PORT_0, packetizer_input_byte);
}


void 
packetizer_arch_writeb(unsigned char c)
{
  rs232_send(RS232_PORT_0, c);
}
