#include <minix/drivers.h>
#include <minix/syslib.h>
#include <lcom/lcf.h>

#include <lcom/lab2.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8254.h"

//Usage of extern so that this global variable
//can be used in both lab2.c and timer.c
extern int _int_handler;


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//Test code that displays the specified field of status/configuration of a specified timer
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {

  uint8_t st; //timer's status byte

//Get configuration of timer (saves the timers status byte in st)
//Tests if works
  if(timer_get_conf(timer, &st) == OK )
  {
    //Displays configuration specified in the "field"
    //Tests if works
    if(timer_display_conf(timer,st,field) == OK)
      return 0;
    else
      return 1;
  }
  else
    return 1;
}

//Test if the specified timer generated a time base with a given frequency
int(timer_test_time_base)(uint8_t timer, uint32_t freq) {

  //Changes frequency of a timer, testing for errors
  if (timer_set_frequency(timer,freq) == OK)
    return 0;
  else
    return 1;
}

//Tests if the interrupts are handled correctly and checks it giving the elapsed time
//doing it during the time passed in the argument time
int(timer_test_int)(uint8_t time) {
  //seconds passed handling interruptions
  int sec_ints = 0;
  //interprocess communication status
  int ipc_status;
  //message sent by the driver controller
  message msg;
  //the mask that will check if the interrupts are related to timer 0
  uint8_t irq_set;
  //will check if driver_receive function works correctly
  int r;
  //Tries to subscribe interruptions for timer 0
  if(timer_subscribe_int(&irq_set) != OK)
    return 1;
  //loop that handles interruptions, checking for them, handling them and increasing the counter
  while( sec_ints < time )
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
        case HARDWARE : // hardware interrupt notification
           if (msg.m_notify.interrupts & irq_set)
           {
             //Calls the int handler to increase the counter
             timer_int_handler();
             //Checks if has been handling interrupts for a second
             //printing a message indicating that
             if(_int_handler % 60 ==0) {
              timer_print_elapsed_time();
              sec_ints++;
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
  }
  //Checks if the unsubscribing of the timer interrupts are done without problems, handling negative results
  if (timer_unsubscribe_int() == OK)
    return 0;
  else
    return 1;
}

//Returns the LSB of a 2 byte integer
//Information to change the frequency to be used in timer_set_frequency
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  uint16_t mask = 0x00FF; //mask to preserve the LSB of val

  *lsb = (uint8_t) (val & mask);
  return 0;
}

//Returns the MSB of a 2 byte integer
//Information to change the frequency to be used in timer_set_frequency
int(util_get_MSB)(uint16_t val, uint8_t *msb) {

  //move the MSBb of InformationChangeFreq to the LSB
  //Cast in unit8_t to get the initial MSB

  uint16_t mask = 0x00FF;//mask to preserve the MSB of val (that will be moved to LSB)
  *msb = (uint8_t) ((val >> 8) & mask);
  return 0;
}
