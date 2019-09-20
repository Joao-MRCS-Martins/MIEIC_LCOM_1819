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
  *bit_no = BIT(hook_id);

  if(sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id) == OK)
    return 0;
  else
  {
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }
}

// auxiliary to kbd_print_no_sysinb
//counts number of sys_in calls
int (sys_inb_cnt)(port_t port, unsigned long *byte)
{
  if(sys_inb(port,(uint32_t*) byte) != OK)
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
    return 0;
  else
  {
    printf("Error: Unable to unsubscribe interruption. ");
    return 1;
  }

}

/*
1. Reads info from status register
2. Loads information(data) from output buffer
3. Checks for errors
*/
int (read_scancode)(uint8_t *data)
{
  unsigned long stat;

    //loads information from status register
    if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register.");
      return 1;
    }
    //checks if information is available
    if( stat & OBF ) 
    {
      if(sys_inb_cnt(OUT_BUF, (unsigned long *) data) != OK) 
      {
        //loads information from output buffer to data 
        printf("Error: couldn't read the scancode from out buffer.");
        return 1;
      }
      //Checks the status for parity, timeout errors or if AUX is set
      if ((stat & (PAR_ERR | TO_ERR | AUX_SET)) == 0 ) 
      {
        return 0;
      }
      else
      {
        printf("Error: Parity | Timeout  | Aux , error occured."); 
        return 1;
      }
    }
    else{
      printf("Error: No information available for reading"); 
     return 1; 
    }
  

}

//interrupt handler in C
 void (kbc_ih)(void) 
 {
    ih_bool = read_scancode(&data);
 }



////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Polling functions ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//Issue the command to enable keyboard interrupts to KBC
int (issue_cmd_b)(unsigned long cmdb)
{
  unsigned long stat;
  int n_tries = 0;
  //tries to issue the command responsible for sending the command byte
  if (issue_command(ISSUE_CMD_B)!= OK)
    return 1;
  //loops while the n_tries dont reach the maximum amount of tries
  while (n_tries < MAX_TRIES)
  {
    //checks again the status register
    if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register. \n");
      return 1;
    }
    //checks if the input buffer is not full
    if ((stat & IBF) == 0 )
    {
      //tries to issue the command byte to the address at 0x64
      if(sys_outb(IN_BUF_ARG, cmdb) != OK)
      {
        printf("Error: couldn't write command byte. \n");
        return 1;
      }
      else 
        return 0;
    }
    n_tries++;
  }
  printf("Error: the Command byte couldn't be issued after all %d tries. \n",MAX_TRIES);
  return 1;
}


////////////////////Read the command byte from out buffer/////////////////////////////////


//auxiliary to read_cmd_b
 //tries to issue the command responsible for reading the command byte
int (issue_command)(unsigned long cmd)
{
  unsigned long stat;
  //current number of tries to issue the command
  int n_tries = 0;

  //while the tries dont exceed the maximum number established it will try to issue command
  while( n_tries < MAX_TRIES ) 
  {
    //reads the status register from the right address
    if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register. \n");
      return 1;
    }  
    //checks if the input buffer is not full
    if( (stat & IBF) == 0 ) 
    {
      //tries to issue the command to KBC
      if(sys_outb(IN_BUF_CMD, cmd) != OK) {
        printf("Error: couldn't write the command on KBC. \n");
        return 1; //no args command
      }
      else
        return 0;
    }
    n_tries++;
  }
  printf("Error: the Command couldn't be issued after all tries.");
  return 1;
}

 //Tries to read the command byte from out buffer
int (read_cmd_b)(unsigned long *cmdb)
{
  unsigned long stat;
  
  //tries to issue the command responsible for reading the command byte
  if (issue_command(READ_CMD_B) != OK)
    return 1;
  //checks the status register
  if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register.");
      return 1;
    }
  //looks out for parity errors and timeout errors 
  if (stat & (PAR_ERR | TO_ERR ))
  {
    printf("Error: Parity or timeout error occured.");
    return 1;
  }
  //tries to read the command byte from the out buffer
  if (sys_inb_cnt(OUT_BUF,cmdb)!= OK) {
    printf("Error: couldn't read the command byte from out buffer.");
    return 1;
  }
  else
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////

int read_scancode_pl(uint8_t *data)
/*
1. Reads information from status register
2. Checks if there is information available in output buffer
       - if no information available -> waits and tries again 1.
3. Loads information 
4. Checks for errors
*/
{
   unsigned long stat;
while(1)  // loop while 8042 output buffer is empty 
{
    //loads information from status register
    if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register.");
      return 1;
    }
    //checks if information is available
    if( stat & OBF ) 
    {
       //loads information from output buffer to data 
      if(sys_inb_cnt(OUT_BUF, (unsigned long *) data) != OK) 
      {
        printf("Error: couldn't read the scancode from out buffer.");
        return 1;
      }
      //Checks the status for parity, timeout errors or if AUX is set
      if ((stat & (PAR_ERR | TO_ERR | AUX_SET)) == 0 ) 
      {
        return 0;
      }
      else
      {
        printf("Error: Parity | Timeout  | Aux , error occured."); 
        return 1;
      }
    }
    else{
    //wait time before checking again if output buffer is full
     tickdelay(micros_to_ticks(WAIT_KBC)); 
    }
}
}






