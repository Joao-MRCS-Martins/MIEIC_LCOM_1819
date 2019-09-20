#ifndef _MOUSE_H_ 

#include "i8254.h"

#include "keyboard.h"
#include "lcom/timer.h"

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();
int writemousecmd(uint32_t mcmd);
int trymousecmd(uint32_t mcmd);


//int fill_struct(uint8_t data,int *size, struct packet *pp);


#endif // _MOUSE_H_
