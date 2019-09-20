#include "lcom/timer.h"
#include <lcom/lcf.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>

#include "mouse.h"

static int hook_id = 12;
extern uint8_t data;
extern int ih_bool;

//gesture buttons checker
static bool lb_pressed = false;
static bool rb_pressed = false;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) == OK)
    return 0;
  else {
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) == OK)
    return 0;
  else {
    printf("Error: Unable to unsubscribe interruption. ");
    return 1;
  }
}


//write commands for mouse
int write_mouse_cmd(uint32_t mcmd) {

  //write 0xD4 to 0x64
  kbc_issue_command(ISSUE_BY2MOUSE, IN_BUF_CMD);
  //write command to 0x60
  kbc_issue_command(mcmd, IN_BUF_ARG);

  uint32_t data;
  //reads data sent by the outputbuffer to check for errors
  read_kbc(&data, OUT_BUF);
  
  /*
  checks for errors
  ACK - no errors
  NACK - invalid byte (try again)
  ERROR - second consecutive invalid byte
  */

  if (data != ACK) {
    if (data == ERROR) {
      return 2;
    }
    else
      return 1;
  }
  else
    return 0;
}

int try_mouse_cmd(uint32_t mcmd) {

  //repeats mouse command if NACK, stop with error if ERROR and stops with succes with ACK
  while (1) {

    int flag = write_mouse_cmd(mcmd);

    if (flag == 1)
      continue;
    else if (flag == 2)
      return 1;
    else if (flag == 0)
      return 0;
  }
}

void(mouse_ih)() {
  ih_bool = read_information(&data, MOUSE_IRQ);
}

//function to detect mouse related events
struct mouse_ev *(mouse_det_evt)(struct packet *pp) {
  enum mouse_ev_t et = BUTTON_EV; //for initialization purposes only, not meaningful
  struct mouse_ev *evt = malloc(sizeof *evt);

  if (pp->delta_x != 0 || pp->delta_y != 0) {
    et = MOUSE_MOV;
    (*evt).type = et;
    (*evt).delta_x = pp->delta_x;
    (*evt).delta_y = pp->delta_y;
  }
  else if (pp->lb && !pp->rb && !pp->mb) {
    et = LB_PRESSED;
    lb_pressed = true;
    (*evt).type = et;
  }
  else if (pp->rb && !pp->lb && !pp->mb) {
    et = RB_PRESSED;
    rb_pressed = true;
    (*evt).type = et;
  }
  else if (!pp->rb && !pp->lb && !pp->mb && lb_pressed) {
    et = LB_RELEASED;
    lb_pressed = false;
    (*evt).type = et;
  }
  else if (!pp->rb && !pp->lb && !pp->mb && rb_pressed) {
    et = RB_RELEASED;
    rb_pressed = false;
    (*evt).type = et;
  }
  else {
    et = BUTTON_EV;
    (*evt).type = et;
  }

  return evt;
}
