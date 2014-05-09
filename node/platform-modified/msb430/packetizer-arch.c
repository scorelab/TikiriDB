#include "packetizer.h"
#include "dev/rs232.h"


void 
packetizer_arch_init(void)
{
  rs232_set_input(packetizer_input_byte);
}


void 
packetizer_arch_writeb(unsigned char c)
{
  rs232_send(c);
}
