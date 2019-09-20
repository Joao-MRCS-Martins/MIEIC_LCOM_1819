#include <lcom/lcf.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>

//Group added headers
#include "credits.h"
#include "game.h"
#include "keyboard.h"

extern int ipc_status;
message msg;
uint8_t r;
uint8_t irq_set_key;
uint8_t data;
int ih_bool;
int _int_handler;
uint8_t sc_code[2];
int size;
bool make;

int(show_image)() {
  clean_screen();
  clean_tmp();

  Graphic *g = (Graphic *) malloc(sizeof(Graphic));

  g->image = loadBitmap("/home/lcom/labs/proj/resources/images/lcomcredits2.bmp");

  drawBitmap(g->image, 0, 0, ALIGN_LEFT);

  memcpy(get_mem(), get_tmp(), get_vram_size());

  if (keyb_subscribe_int(&irq_set_key) != OK)
    return 1;

  /*
Exit game if ESC is pressed
*/
  while (data != ESC_BREAK) {

    if ((r = driver_Receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          //handles keyboard interruptions
          if (msg.m_notify.interrupts & BIT(irq_set_key)) {

            kbc_ih();
            if (ih_bool != 0) //verify if handler is sucessful
              break;

            sc_code[size] = data;
            size++;
            make = !(data & BREAK_CODE);

            if (size == 2) { //game only processes one byte codes
              size = 0;
            }
            else if (size == 1) {
              if (data != TWO_B_CODE) {
                size = 0;
              }
            }
          }
        default:
          break; // no other notifications expected: do nothing
      }
    }

    else {
      printf("Failed to receive notification from ipc. \n");
      return 1;
    }
  }

  if (keyboard_unsubscribe_int() != OK)
    return 1;

  deleteBitmap(g->image);

  return 0;
}
