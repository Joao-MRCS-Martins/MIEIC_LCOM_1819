#include <minix/drivers.h>
#include <minix/syslib.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>


#include <stdint.h>

#include "i8254.h"

//global variables
int _int_handler = 0;//counts number of interruptions in the timer
static int hook_id = 0;


//Subcribes the timer 0 interruptions
//Returns 0 if everything goes as expected, returns 1 if it encountered errors
int (timer_subscribe_int)(uint8_t *bit_no)
{
  hook_id=0; //to make sure the variable used for check irq is the timer's
  
  //@param bit_no will store the mask created by passing the hook_id argument bitwise
  *bit_no = hook_id;

  //Tests if the set of policy on the timer 0 IRQ is done without issues and handles errors
  if (sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) == OK)
    return 0;
  else
  {
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }

}
//Unsubscribes the timer 0 interruptions
//Returns 0 if everything goes as expected, returns 1 otherwise
int (timer_unsubscribe_int)()
{
  //Tests if the remove of the policy on the timer 0 works fine and handles errors
  if (sys_irqrmpolicy(&hook_id) == OK)
    return 0;
  else
  {
    printf("Error: Unable to unsubscribe interruption. ");
    return 1;
  }

}

//Handles the timer interruptions
//Increases the global variable that works as counter of the interruptions handled
void (timer_int_handler)() {
  _int_handler++;
}
