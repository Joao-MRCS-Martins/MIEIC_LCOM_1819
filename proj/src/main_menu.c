#include <lcom/lcf.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>

//Group added headers
#include "main_menu.h"
#include "mouse.h"
#include "game.h"

extern int _int_handler;
extern uint8_t data;
extern int state;

//variables for mouse information
static bool cx;
static bool cy;

int handling_interruptions(struct packet *pp, uint8_t irq_set_mouse) {
  int ipc_status = 0;
  message msg;
  uint8_t r;
  int size = 0;

  while (true) {
    // Get a request message.
    if ((r = driver_Receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) //received notification
    {
      switch (_ENDPOINT_P(msg.m_source)) {
        // hardware interrupt notification
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irq_set_mouse)) //mouse interrupt handler
          {
            mouse_ih();

            switch (size) {
              case 0:

                if (FLAG_BYTE1 & data) {
                  (*pp).bytes[size] = data;
                  (*pp).rb = (data & RB);
                  (*pp).lb = (data & LB);
                  (*pp).mb = (data & MB);
                  (*pp).x_ov = (data & X_OVF);
                  (*pp).y_ov = (data & Y_OVF);

                  //cx and cy save byte SIGN to verify if bytes 1 and 2 are complement or not
                  cx = (data & X_SIGN);
                  cy = (data & Y_SIGN);
                  size++;
                }
                else {
                  size = 0;
                }

                break;
              case 1:

                (*pp).bytes[size] = data;
                if (cx) {
                  (*pp).delta_x = data - 256;
                }
                else {
                  (*pp).delta_x = data;
                }

                size++;
                break;

              case 2:
                (*pp).bytes[size] = data;
                if (cy) {
                  (*pp).delta_y = data - 256;
                }
                else {
                  (*pp).delta_y = data;
                }
                return 0;
                //break;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
      printf("Failed to receive notification from ipc. \n");
      return 1;
    }
  }
}

void draw_pointer(uint16_t x, uint16_t y, uint16_t color) {

  if (x + SIZE > get_h_res())
    x = get_h_res() - SIZE;
  if (y + SIZE > get_v_res())
    y = get_v_res() - SIZE;
  vg_draw_rectangle(x, y, CURSOR_SIZE, CURSOR_SIZE, color);
}

int detect_options_mouse() {

  //draw image on screen
  Background *background = (Background *) malloc(sizeof(Background));
  background->image = loadBitmap("/home/lcom/labs/proj/resources/images/mainmenu3.bmp");
  drawBitmap(background->image, 0, 0, ALIGN_LEFT);
  memcpy(get_mem(), get_tmp(), get_vram_size());

  int state = 0;
  struct packet pp;
  struct mouse_ev *evt;
  uint8_t irq_set_mouse;

  //mouse cursor initial position
  int16_t x_pos = get_h_res() / 2;
  int16_t y_pos = get_v_res() / 2;

  if (try_mouse_cmd(STREAM_MODE) != OK)
    return 1;

  if (try_mouse_cmd(EN_DATA_REP) != OK)
    return 1;

  if (mouse_subscribe_int(&irq_set_mouse) != OK)
    return 1;

  while (1) {
    clean_tmp();

    handling_interruptions(&pp, irq_set_mouse);

    evt = mouse_det_evt(&pp);
    if (evt->type == MOUSE_MOV) {

      x_pos += evt->delta_x * CURSOR_SPEED;
      y_pos -= evt->delta_y * CURSOR_SPEED;
    }

    if (x_pos < 0)
      x_pos = 0;
    if (x_pos > get_h_res())
      x_pos = get_h_res();
    if (y_pos < 0)
      y_pos = 0;
    if (y_pos > get_v_res())
      y_pos = get_v_res();

    drawBitmap(background->image, 0, 0, ALIGN_LEFT);

    if ((x_pos > PLAY_X) && (x_pos < (PLAY_X + PLAY_WIDTH)) && (y_pos > PLAY_Y) && (y_pos < (PLAY_Y + PLAY_HEIGHT))) {

      if (evt->type == LB_PRESSED) {
        state = 1;
        break;
      }
      else {
        vg_draw_rect_empty(PLAY_X, PLAY_Y, PLAY_WIDTH, PLAY_HEIGHT, LIGHT_BLUE);
      }
    }
    else if ((x_pos > CREDITS_X) && (x_pos < (CREDITS_X + CREDITS_WIDTH)) && (y_pos > CREDITS_Y) && (y_pos < (CREDITS_Y + CREDITS_HEIGHT))) {

      if (evt->type == LB_PRESSED) {
        state = 2;
        break;
      }
      else {
        vg_draw_rect_empty(CREDITS_X, CREDITS_Y, CREDITS_WIDTH, CREDITS_HEIGHT, LIGHT_BLUE);
      }
    }
    else if ((x_pos > QUIT_X) && (x_pos < (QUIT_X + QUIT_WIDTH)) && (y_pos > QUIT_Y) && (y_pos < (QUIT_Y + QUIT_HEIGHT))) {

      if (evt->type == LB_PRESSED) {
        state = 3;
        break;
      }
      else {
        vg_draw_rect_empty(QUIT_X, QUIT_Y, QUIT_WIDTH, QUIT_HEIGHT, LIGHT_BLUE);
      }
    }
    else {

      vg_draw_rect_empty(PLAY_X, PLAY_Y, PLAY_WIDTH, PLAY_HEIGHT, BLACK);
      vg_draw_rect_empty(CREDITS_X, CREDITS_Y, CREDITS_WIDTH, CREDITS_HEIGHT, BLACK);
      vg_draw_rect_empty(QUIT_X, QUIT_Y, QUIT_WIDTH, QUIT_HEIGHT, BLACK);
    }
    draw_pointer(x_pos, y_pos, choose_color(rand() % 7 + 1));

    //update screen
    memcpy(get_mem(), get_tmp(), get_vram_size());
  }

  deleteBitmap(background->image);

  //returning to default state by unsubscribing interruptions
  if (mouse_unsubscribe_int() != OK)
    return 1;

  //and disabling the data reporting
  if (try_mouse_cmd(DIS_DATA_REP) != OK)
    return 1;

  return state;
}
