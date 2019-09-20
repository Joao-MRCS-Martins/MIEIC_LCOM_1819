#include <lcom/lcf.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>

//Group added headers
#include "keyboard.h"

int cnt = 0;
static int hook_id = 1;
uint8_t data = 0;
int ih_bool = 0;

//subscribe keyboard interrupt
//avoids conflit with Default Interrupt Handler
int(keyb_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;

  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) == OK) {
    return 0;
  }
  else {
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }
}

//Unsubscribes keyboard interruptions and handles errors
int(keyboard_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) == OK) {
    return 0;
  }
  else {
    printf("Error: Unable to unsubscribe interruption. ");
    return 1;
  }
}

//interrupt handler in C
void(kbc_ih)(void) {
  ih_bool = read_information(&data, KEYBOARD_IRQ);
}

uint8_t sc_code[2]; //array for scancode bytes
int size = 0;       //size of scancode
bool make;          //true- makecode false-breakcode

