#ifndef _KEYBOARD_H_ 

#include "i8254.h"
#include "lcom/timer.h"

/* wrapper function*/
#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q) 
#endif

////////////////////////////////////////////////////////////////////

int keyb_subscribe_int(uint8_t *bit_no);
int keyboard_unsubscribe_int();
/*int issue_command(uint32_t cmd);
int read_scancode(uint8_t *data, uint8_t IRQ);
int read_cmd_b(uint32_t *cmdb);
int issue_cmd_b(uint32_t cmdb);
int read_scancode_pl(uint8_t *data,uint8_t IRQ, uint16_t period);*/

#endif // _KEYBOARD_H_
