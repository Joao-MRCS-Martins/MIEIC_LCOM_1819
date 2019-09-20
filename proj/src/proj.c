// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

// Any header files included below this line should have been created by you
#include "credits.h"
#include "game.h"
#include "main_menu.h"
#include "mouse.h"
#include "vbe.h"
#include "videog.h"

int state;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(proj_main_loop)(int UNUSED(argc), char *UNUSED(argv[])) {

  int state = MAIN_MENU;
  vg_init(DIRECT);

  while (1) {
    switch (state) {
      case 0: {
        state = detect_options_mouse();
        clean_screen();
        break;
      }
      case 1: {

        if (game_main() != 0)
          return 1;

        state = 0;
        break;
      }
      case 2:
        if (show_image() != 0)
          return 1;

        state = 0;
        break;
      case 3:
        vg_exit();
        return 0;
      default: {
        printf("Error: invalid state. \n");
        break;
      }
    }
  }

  return 0;
}
