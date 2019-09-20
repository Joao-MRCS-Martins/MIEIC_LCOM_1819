#include <lcom/timer.h>
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include <i8042.h> 
#include <keyboard.h>

extern int cnt; //counts number of sys_in calls
extern uint8_t data; //variable for scancodes information
extern int ih_bool; //check for errors in interrupt handler
extern int _int_handler; //counts number of interruptions in timer
uint8_t sc_code [2]; //array for scancode bytes
int size=0; //size of scancode
bool make; //true- makecode false-breakcode

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

int (kbd_test_scan)(bool assembly) {
  /*1.Subscribe
   2.Handles interruptions
   3.Print number of sys_in calls
   4.Unsubscribe keyboard
  */

  int ipc_status=0;
  message msg;
  uint8_t r;
  uint8_t irq_set;

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
            if (msg.m_notify.interrupts & irq_set)
            {
              if (assembly){
                //handles interruption with assembly code
                kbc_asm_ih(); 
              }
              else
              {
               //handles interruption with C code
                kbc_ih();
                kbd_print_no_sysinb(cnt); //increment cnt
              }
              
              //checks for errors in interrupt handler
              if(ih_bool != 0)
                break;
              
              sc_code[size] = data;
              size++; 

              make = !(data & BREAK_CODE);

              //////////////////////////////////////////
              if(size == 2) 
              {
                kbd_print_scancode(make,size,sc_code);
                size = 0;
              } 
              else if(size==1)
              {
                if (data != TWO_B_CODE)
                {
                    kbd_print_scancode(make,size,sc_code);
                    size= 0;
                }
              }
              ///////////////////////////////////////////
    
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int (kbd_test_poll)()
{
  /* 1.Handles interruptions
   2.Print number of sys_in calls
   3.Enable interruptions
  */

  unsigned long cmdb;
  
  
  while(data != ESC_BREAK)
  {
    if(read_scancode_pl(&data) !=OK)
       return 1;

    sc_code[size] = data;
    size++;
    make = !(data & BREAK_CODE);
    //////////////////////////////
    if(size == 2) 
    {
      kbd_print_scancode(make,size,sc_code);
      size = 0;
    } 
    else if(size==1)
    {
      if (data != TWO_B_CODE)
      {
        kbd_print_scancode(make,size,sc_code);
        size = 0;
      }
    }
    ////////////////////////
  }
  

  kbd_print_no_sysinb(cnt);

  //Read the command byte from out buffer
  if (read_cmd_b(&cmdb) != OK) 
    return 1;
  
  //Sets the bit responsible for enabling interrupts for the keyboard
  cmdb |= EN_INT_KBD;

  //Issue the command to enable keyboard interrupts to KBC
  if (issue_cmd_b(cmdb) != OK) 
     return 1;
  else 
    return 0;
  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int (kbd_test_timed_scan)(uint8_t n) {
   /*1.Subscribe
   2.Handles interruptions
      -keyboard
      -timer
   3.Print number of sys_in calls
   4.Unsubscribe keyboard
  */
  int ipc_status=0;
  message msg;
  uint8_t r;
  uint8_t irq_set_tim;
  uint8_t irq_set_key;


  //subscribe KBC
   //avoids conflit with Default Interrupt Handler
  if(keyb_subscribe_int(&irq_set_key) != OK) 
      return 1;
  //subscribe timer
  if(timer_subscribe_int(&irq_set_tim) != OK)
    return 1;
    while(data != ESC_BREAK && (_int_handler/60) < n)
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
            if (msg.m_notify.interrupts & irq_set_key)
            {
              kbc_ih();
              
              
              if(ih_bool != 0)
                break;

              sc_code[size] = data;
              size++;

              make = !(data & BREAK_CODE);

              ////////////////////////////
              if(size == 2) 
              {
                kbd_print_scancode(make,size,sc_code);
                size = 0;
              } 
              else if(size==1)
              {
                if (data != TWO_B_CODE)
                {
                    kbd_print_scancode(make,size,sc_code);
                    size= 0;
                }
              }
              _int_handler = 0;
              ///////////////////////////
            }
            else 
            //handles timer interruptions
            if (msg.m_notify.interrupts & irq_set_tim)
            {
             //Calls the int handler to increase the counter
             timer_int_handler(); 
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
    }
    
  if(timer_unsubscribe_int() != OK)
    return 1;
  if(keyboard_unsubscribe_int() != OK)
    return 1;
  
  return 0;
}
