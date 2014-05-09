
#include "tikiridb.h"
#include "attr-table.h"
#include "nw-types.h"

#include "dev/sht11.h"

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else /* DEBUG  */
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
int 
get_node_address(attr_data_t * data_ptr)
{
  data_ptr->data_bytes[0] = rimeaddr_node_addr.u8[0];
  data_ptr->data_bytes[1] = rimeaddr_node_addr.u8[1];
  data_ptr->data_bytes[2] = 0;
  data_ptr->data_bytes[3] = 0;
  return 0;
}
/*---------------------------------------------------------------------------*/
int
compare_node_address(attr_data_t * A, attr_data_t * B, uint8_t op) 
{
  rimeaddr_t * addr1 = (rimeaddr_t *)A;
  rimeaddr_t * addr2 = (rimeaddr_t *)B;
  char cmp_status;

  cmp_status = rimeaddr_cmp(addr1, addr2);
  switch(op) {
    case EQ:
      return cmp_status;
    case NEQ:
      return !cmp_status;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
int 
get_temp(attr_data_t * data_ptr)
{
  uint16_t temp;
  temp = (uint16_t)(-39.60 + 0.01 * sht11_temp());
  PRINTF("temp %d C\n", temp);
  hton_leuint16(data_ptr, temp);
  
  return 0;
}
/*---------------------------------------------------------------------------*/
int
compare_temp(attr_data_t * A, attr_data_t * B, uint8_t op) 
{

  uint16_t u_A = ntoh_leuint16(A);
  uint16_t u_B = ntoh_leuint16(B);
  switch(op) {
    case EQ:
      return u_A == u_B;
    case NEQ:
      return u_A != u_B;
    case GT:
      return u_A > u_B;
    case GE:
      return u_A >= u_B;
    case LT:
      return u_A < u_B;
    case LE:
      return u_A <= u_B;
  }

  return 0;
}
/*---------------------------------------------------------------------------*/
int 
get_hum(attr_data_t * data_ptr)
{
  uint16_t hum;
  static unsigned rh;
  rh = sht11_humidity();
  hum = (uint16_t) (-4 + 0.0405*rh - 2.8e-6*(rh*rh));
  PRINTF("humidity %d%%\n", hum);
  hton_leuint16(data_ptr, hum);
  
  return 0;
}
/*---------------------------------------------------------------------------*/
void 
tikiridb_arch_init(void)
{
  /* Add attribute "node address" */
  add_attr_entry(1, get_node_address, compare_node_address);
  /* Add attribute "temperature" */
  add_attr_entry(2, get_temp, compare_temp);
  /* Add attribute "temperature" */
  add_attr_entry(3, get_hum, compare_temp);
}
