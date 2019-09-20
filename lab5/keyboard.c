#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include <stdint.h>
#include "lcom/timer.h"

//Group added headers
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "kbc.h"

//information of this variables in lab3.c
int cnt = 0; 
static int hook_id = 1;
uint8_t data = 0;
int ih_bool = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////

//subscribe keyboard interrupt
//avoids conflit with Default Interrupt Handler
int (keyb_subscribe_int)(uint8_t *bit_no)
{
  *bit_no = hook_id;

  if(sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id) == OK)
  {
    return 0;
  }
  else
  {
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }
}

// auxiliary to kbd_print_no_sysinb
//counts number of sys_in calls
int (sys_inb_cnt)(port_t port, uint32_t *byte)
{
  if(sys_inb(port,byte) != OK)
   return 1;
  else
  {
  cnt++; //increments global variable
  return 0;
  }

}

//Unsubscribes keyboard interruptions and handles errors
int (keyboard_unsubscribe_int)()
{
  if (sys_irqrmpolicy(&hook_id) == OK)
  {
    return 0;
  }
  else
  {
    printf("Error: Unable to unsubscribe interruption. ");
    return 1;
  }

}

//interrupt handler in C
 void (kbc_ih)(void) 
 {
    ih_bool = read_information(&data,KEYBOARD_IRQ);
 }


uint8_t sc_code [2]; //array for scancode bytes
int size=0; //size of scancode
bool make; //true- makecode false-breakcode

int (kbd_scan_esc)() {
  /*1.Subscribe
   2.Handles interruptions
   3.Print number of sys_in calls
   4.Unsubscribe keyboard
  */
  int ipc_status=0;
  message msg;
  uint8_t r;
  uint8_t irq_set =1;

   //subscribe KBC
   //avoids conflit with Default Interrupt Handler
  if(keyb_subscribe_int(&irq_set) != OK) 
      return 1;

    //exit condition 
    while(data != ESC_BREAK)
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
          //hardware interrupt notification
          case HARDWARE : 
            //handles keyboard interruptions
            if (msg.m_notify.interrupts & BIT(irq_set))
            {
              
               //handles interruption with C code
                kbc_ih();
              
              //checks for errors in interrupt handler
              if(ih_bool != 0)
                break;

            }
            break;
          default :
            break; // no other notifications expected: do nothing
        }
      }
      //handling errors
      else
      {
        printf("Failed to receive notification from ipc. \n");
        return 1;
      }
      if(ih_bool != 0)
        break;

        
    }

    keyboard_unsubscribe_int();
  
   return 0;
}




