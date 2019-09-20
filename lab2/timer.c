#include <minix/drivers.h>
#include <minix/syslib.h>
#include <lcom/lcf.h>
#include <lcom/timer.h>


#include <stdint.h>

#include "i8254.h"

//global variables
int _int_handler = 0;//counts number of interruptions in the timer
int hook_id = 0;

//
//Changes the operating frequency of a timer:
//1. Reads read-back command to preserve mode and BCD/binary of timer
//2. Programming timer by sending the right control word to the control register
//3. Send the information for the new frequency
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  //This cap is because the value to be put on the timer cannot surpass the 16-bit range
  if (freq < 19)
  {
    printf("The frequency given is too low, due to data types limitations. \n");
    return 1;
  }

//Initialization of variables
  uint8_t controlWord = TIMER_LSB_MSB	; //controlworld to programm timer in LSB_after_MSB mode
  uint8_t st; //status byte to be filled
  uint8_t mask; //mask for bits 0 to 3


//Get the configuration from the timer
//In order to preserve the bits 0 to 3 (when creating the control word)
if(timer_get_conf(timer, &st) == 0)
  {
    mask = st & (BIT(0) | BIT(1) | BIT(2) |  BIT(3)); //preserves bits 0 to 3

//Creates the controlword for the control register
//The control word:
//    -- preserves the bits 0 to 3 from the timer's configuration (mask)
//    -- has the right bits for the timer to use
//    -- has the LSB_after_MSB mode - necessary to change frequency
    switch(timer)
    {
      case 0 :
      {
        controlWord |= mask | TIMER_SEL0;
        break;
      }
      case 1 :
      {
        controlWord |= mask | TIMER_SEL1;
        break;
      }
      case 2 :
      {
        controlWord |= mask | TIMER_SEL2;
        break;
      }
      default:
      {
        printf("Invalid timer! Choose a timer from 0 to 2. \n");
        return 1;
      }
    }

    //
    //Programm timer to change the frequency
    if(sys_outb(TIMER_CTRL, controlWord) == OK)

    {
      //Information we need to send to change frequency
      uint16_t InformationChangeFreq = (TIMER_FREQ /freq);

      //util_get_LSB and util_get_MSB get the lsb or msb bit of the InformationChangeFreq
      //and save it in uint8_t
      uint8_t lsb;
      uint8_t msb;

      if(util_get_LSB(InformationChangeFreq,&lsb) !=0)
      {
        printf("Error: Invalid conversion");
        return 1;
      }
      if(util_get_MSB(InformationChangeFreq, &msb) !=0)
      {
        printf("Error: Invalid conversion");
        return 1;
      }


      //Send frequency to timer;
      //first send the LSB and then the MSB
      if(sys_outb(TIMER_0 + timer,lsb) == OK)
      {
          if(sys_outb(TIMER_0 + timer,msb) == OK)
            return 0;
          else
          {
            printf("Error: Couldn't load MSB to timer. ");
            return 1;
          }
      }
      else
      {
        printf("Error: Couldn't load LSB to timer. ");
        return 1;
      }
    }
    else
    {
      printf("Error: Couldn't access Control Register. ");
      return 1;
    }

  }
  else
  {
  printf("Error: Couldn't run timer_get_conf. ");
  return 1;
  }
}


//Subcribes the timer 0 interruptions
//Returns 0 if everything goes as expected, returns 1 if it encountered errors
int (timer_subscribe_int)(uint8_t *bit_no)
{

  //@param bit_no will store the mask created by passing the hook_id argument bitwise
  *bit_no = BIT(hook_id);

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

//Creates the Read Back Command with given timer and writes it on the Control register
int (timer_get_conf)(uint8_t timer, uint8_t *st)
{
    // rbc will store the read back command; the 32 bits size is only for resuse purposes
    uint32_t rbc = TIMER_RB_CMD | TIMER_RB_COUNT_ |TIMER_RB_SEL(timer);
    // this flag will be set in case the reading from the timer config and wrting on *st messes up
    bool stbflag = false;

    //Tests if the writing of the read back command to the control register
    //is done correctly, handling errors
    if (sys_outb(TIMER_CTRL, rbc) == OK)
    {
      //Checks which timer was selected on the function, acting different accordingly
      //Then proceeds to read the timer configuration from selected timer and stores it on the *st parameter
      //Also handles errors when getting the timer configuration
      //If the timer selected isn't valid, handles the error
      switch(timer)
      {
        case 0 :
          if(sys_inb(TIMER_0, &rbc) != OK)
          stbflag = true;
          *st = (uint8_t) rbc;
          break;
        case 1 :
          if(sys_inb(TIMER_1, &rbc) != OK)
          stbflag = true;
          *st = (uint8_t) rbc;
          break;
        case 2 :
          if(sys_inb(TIMER_2, &rbc) != OK)
          stbflag = true;
          *st = (uint8_t) rbc;
          break;
        default :
          printf("Error: invalid timer. ");
          return 1;
      }
      if (stbflag)
      {
        printf("Error: couldn't write the status byte to the specified address. ");
        return 1;
      }
      else
        return 0;
    }
    else
    {
      printf("Error: couldn't write read back command to timer control register. ");
      return 1;
    }

}

// Gets the timer configuration and handles it to display the user the desired part
int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{

//Creates union to use in timer_print_config
union timer_status_field_val var;

//Fill in the field in the union that is specified in the field variable

//Fill all
if(field == all)
{
var.byte = st;
//Calls function to print in configuration
if(timer_print_config(timer,field,var) == OK) //calls the printing function and handles errors
  return 0;
else
{
  printf("Error: Couldn't print configuration of timer. ");
  return 1;
}
}


//Fill initial
if(field == initial)
{
if((st & BIT(5)) != 0)
{

  if((st & BIT(4)) != 0)
  {

    var.in_mode = MSB_after_LSB;
  }
  else
    var.in_mode = MSB_only;
}
else if((st & BIT(4)) != 0)
    var.in_mode = LSB_only;
else
    var.in_mode = INVAL_val;
//Calls function to print in configuration
    if(timer_print_config(timer,field,var) == OK) //calls the printing function and handles errors
      return 0;
    else
    {
      printf("Error: Couldn't print configuration of timer. ");
      return 1;
    }
}

//Fill mode
if(field == mode)
{
if((st & BIT(3)) != 0) //bit 3 a 1
  {
    if((st & BIT(2)) != 0) //bit 3=1 ; bit2=1
    {
      if((st & BIT(1)) != 0) //bit 3=1 ; bit2=1; bit1 =1
        var.count_mode = 3;
      else
        var.count_mode = 2 ;//bit 3=1 ; bit2=1; bit1 =0
    }
    else
    {
      if ((st & BIT(1)) != 0)//bit 3=1 ; bit2=0; bit1 =1
        var.count_mode = 5;
      else
        var.count_mode = 4; //bit 3=1 ; bit2=0; bit1 =0
    }
  }
else
{
    if((st & BIT(2)) != 0)//bit 3=0 ; bit2=1;
    {
      if((st & BIT(1)) != 0) //bit 3=0 ; bit2=1; bit1 =1
        var.count_mode = 3;
      else
        var.count_mode = 2 ;//bit 3=0 ; bit2=1; bit1 =0
    }
    else
    {
      if ((st & BIT(1)) != 0) //bit 3=0 ; bit2=0; bit1 =1
        var.count_mode = 1;
      else
        var.count_mode = 0; //bit 3=0 ; bit2=0; bit1 =0
    }
}

//Calls function to print in configuration
if(timer_print_config(timer,field,var) == OK) //calls the printing function and handles errors
  return 0;
else
{
  printf("Error: Couldn't print configuration of timer. ");
  return 1;
}
}

//Fill base
if(field == base)
{
if((st & BIT(0)) != 0)
var.bcd= true;
else
var.bcd = false;
//Calls function to print in configuration
if(timer_print_config(timer,field,var) == OK) //calls the printing function and handles errors
  return 0;
else
{
  printf("Error: Couldn't print configuration of timer. ");
  return 1;
}
}

return 1;

}
