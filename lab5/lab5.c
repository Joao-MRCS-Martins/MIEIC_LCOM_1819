// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "kbc.h"
#include "videog.h"
#include "keyboard.h"
#include "i8042.h"
#include "vbe.h"
extern uint8_t data;
extern int ih_bool;
extern int _int_handler;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) {
  /* 
    try to set initialize vg with given mode

    use timer interruption for waiting 

    and then exit with vg_exit()
    
   */

  if(vg_init(mode) == NULL)
  {
    printf("Error: video graphics failed to initialize. \n");
    return 1;
  }
  
  sleep(delay);

  if(vg_exit() !=0)
  {
    printf("Error: failed to exit graphic mode.\n");
    return 1;
  }
  return 0;

}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

  if (vg_init(mode) == NULL) {
    printf("Error: vg_init failed");
    return 1;
  }

  //check if the drawing works without errors
  //and if it happens unfortunately, skips to vg_exit
  //to allow proper exit and reset graphics mode
  if(vg_draw_rectangle(x, y,width,height,color) !=0) {
    printf("Error: failed to draw shape. \n");
  }
  //the drawing worked, now wait for ESC break code
  else {
  kbd_scan_esc();
  }

  //no matter the function worked as expected or not
  //it must reset graphics mode
  if(vg_exit() !=0)
  {
    printf("Error: failed to exit graphic mode.\n");
    return 1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  
  if (vg_init(mode) == NULL) {
    printf("Error: vg_init failed");
    return 1;
  }

  uint16_t v_res, h_res;
  uint16_t h_rectangle, v_rectangle;

  v_res= get_v_res();
  h_res = get_h_res();

  //height and width of a rectangle 
  h_rectangle = (h_res / no_rectangles);
  v_rectangle = (v_res / no_rectangles);

  uint32_t color;
  uint16_t x, y ;
  x = 0;
  y = 0;
  uint16_t row = 0;
  uint16_t collumn = 0;
  

  //uint16_t size; // height and width of the rectangles
  //uint32_t color;
  for(uint8_t i = 0; i < no_rectangles; i++) 
  {
    for(size_t n = 0; n < no_rectangles; n++)
    {
    color = color_mode(row,collumn,mode,step,first,no_rectangles);
    vg_draw_rectangle(x,y,h_rectangle, v_rectangle,color);

    x += h_rectangle;

     collumn++;

    }
    collumn = 0;
    row++;
    x = 0;
    y += v_rectangle;

  }

  kbd_scan_esc();

  vg_exit();
  return 1;
}

int (video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y) {
 
  if(vg_init(INDEX) == NULL)
  {
    printf("Error: video graphics failed to initialize. \n");
    return 1;
  }

  //using the functions given, create sprite that already maps
  //the passed xpm and fills the necessary fields of the struct
  Sprite* sp = create_sprite(xpm,x,y,0,0);
  if(sp==NULL) {
    printf("Error: failed to create sprite. \n");
    return 1;
  }
  
  //draw the xpm
  draw_sprite(sp);
  
  //wait for ESC break code
  kbd_scan_esc();

  //reset the graphics mode in order to exit
  if(vg_exit() !=0)
  {
    printf("Error: failed to exit graphic mode.\n");
    return 1;
  }
  return 0;
}

int (video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  
  //check if coordinates are in conformity with function specifications
  if((xi!=xf) && (yi!=yf)) {
    printf("Error: Invalid position parameters. \n");
    return 1;
  }

  //check if fr_rate on input exceeds maximum frame rate computable on Minix
  if(fr_rate > MAX_FR_RATE) {
    printf("Error: Max frame rate excedeed. \n");
    return 1;
  }

  //initialize graphics mode indexed (0x105)
  if(vg_init(INDEX) == NULL)
  {
    printf("Error: video graphics failed to initialize. \n");
    return 1;
  }

  //check if the animation will start outside the boundaries of the screen
  if((xi<0) || (xi>get_v_res()) || (yi<0) || (yi > get_h_res()) || (xf<0) || (xf>get_v_res()) || (yf<0) || (yf > get_h_res())) {
    printf("Error: Animation will go out of bounds (screen resolution). \n");
    //return 1;
  }

  //number of interrupts per frame
  uint8_t ints = 60/fr_rate;
  
  //total displacement left
  int displace;

  //irq_set for timer and keyboard
  uint8_t irq_set_tim;
  uint8_t irq_set_keyb;

  //subscribe timer interruptions
  if(timer_subscribe_int(&irq_set_tim) !=0) {
    printf("Error: failed to subscribe timer interrupts. \n");
    return 1;
  }

  //subscribe keyboard interruptions
  if(keyb_subscribe_int(&irq_set_keyb) !=0) {
    printf("Error: failed to subscribe keyboard interrupts. \n");
    return 1;
  }
  
  //create a sprite and map the picture
  Sprite* sp = create_sprite(xpm,xi,yi,0,0);
  if(sp==NULL) {
    printf("Error: failed to create sprite. \n");
    return 1;
  }
  
  //initial drawing of the sprite
  draw_sprite(sp);

  //if the speed is positive means the value will be the speed
  //of the wanted coordinate (either x or y)
  if(speed >=0) {
    
    //check which direction the sprite will "move"
    //whether is along the y-axis
    if(xi==xf) {
      displace = abs(yf-yi);
      if(yi < yf) {
        sp->yspeed = speed;
      }
      else {
        sp->yspeed = -speed;
      }
    }
    //or the x-axis
    else {
      displace = abs(xf-xi);
      if(xi<xf) {
        sp->xspeed = speed;
      }
      else {
        sp->xspeed = -speed;
      }
    }
    //if speed passed is 0, the sprite won't move
    if (speed ==0)
      displace = 0;

    int r;
    int ipc_status;
    message msg;

    while(data != ESC_BREAK )
      {
        // Get a request message.
        if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
        {
          printf("driver_receive failed with: %d", r);
          continue;
        }
        if (is_ipc_notify(ipc_status)) //received notification
        {
          switch (_ENDPOINT_P(msg.m_source))
          {
            // hardware interrupt notification
            case HARDWARE : 
              //handles keyboard interruptions
              if (msg.m_notify.interrupts & BIT(irq_set_keyb))
              {
                kbc_ih();
                
                if(ih_bool != 0)
                  break;
              }
              else 
              //handles timer interruptions
              if (msg.m_notify.interrupts & BIT(irq_set_tim))
              {
                //Calls the int handler to increase the counter
                timer_int_handler();

                //when a frame has passed, update the position and total displacement left
                if(_int_handler%ints ==0) {
                  if(displace > 0) {
                    //"moves" picture
                    //since either xspeed or yspeed will be 0, there is no need to check which coordinate to update
                    sp->x += sp->xspeed;
                    sp->y += sp->yspeed;
                    displace -= speed;
                  }

                  //this condition is to prevent from drawing beyond the wanted position
                  else {
                    sp->x = xf;
                    sp->y = yf;
                  }
                }
              }
            
              break;
            default :
              break; // no other notifications expected: do nothing
          }
        }
        else
        {
          printf("Failed to receive notification from ipc. \n");
          return 1;
        }

        //clean the screen to redraw the sprite on its correct position
        clean_screen();
        //redraw the sprite on the updated position
        draw_sprite(sp);
      }
  }
  else {

    if(xi==xf) {
      displace = abs(yf-yi);
    }
    else {
      displace = abs(xf-xi);
    }
    
    int n_frames=0; //counter of the number of frames passed
    
    int r;
    int ipc_status;
    message msg;

    while(data != ESC_BREAK )
    {
      // Get a request message.
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 )
      {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) //received notification
      {
        switch (_ENDPOINT_P(msg.m_source))
        {
          // hardware interrupt notification
          case HARDWARE : 
            //handles keyboard interruptions
            if (msg.m_notify.interrupts & BIT(irq_set_keyb))
            {
              kbc_ih();
              
              if(ih_bool != 0)
                break;
            }
            else 
            //handles timer interruptions
            if (msg.m_notify.interrupts & BIT(irq_set_tim))
            {
             //Calls the int handler to increase the counter
             timer_int_handler();

              if(_int_handler%ints ==0) {
                
                n_frames++;
                
                if(n_frames == abs(speed)) {
                  n_frames = 0;
                  //if the number of frames necessary for displace the pic by 1 pixel is reached, update the sprite position
                  if(displace > 0) {
                    
                    if(xi==xf) {
                      if(yi <yf)
                        sp->y += 1;
                      else
                        sp->y -= 1;
                    }
                    else {
                      if(xi<xf)
                        sp->x +=1;
                      else
                        sp->x -=1;
                    }
                    displace -=1;
                  }

                  //this condition is to prevent from drawing beyond the wanted position
                  else {
                    sp->x = xf;
                    sp->y = yf;
                  }
                }
              }
            }
           
            break;
          default :
            break; // no other notifications expected: do nothing
        }
      }
      else
      {
        printf("Failed to receive notification from ipc. \n");
        return 1;
      }

      //clean the screen to redraw the sprite on its correct position
      clean_screen();
      //redraw the sprite on the updated position
      draw_sprite(sp);
    }
  }
  
  if(timer_unsubscribe_int() != OK)
    return 1;
  if(keyboard_unsubscribe_int() != OK)
    return 1;

  if(vg_exit() !=0)
  {
    printf("Error: failed to exit graphic mode.\n");
    return 1;
  }
  return 0;
  
}

int (video_test_controller)() {

  vg_vbe_contr_info_t send_info;
  
  if(vbe_get_info_controller(&send_info) != OK)
    printf("Error in vbe_get_info_controller");

  vg_display_vbe_contr_info(&send_info);
  
  
  return 1;
}  
