#include "lcom/timer.h"
#include <lcom/lcf.h>
#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <stdint.h>

//Group added headers
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "mouse.h"

int read_kbc(uint32_t *data, int port) {

  // WHILE 1
  // read status
  // if OBF
  // continue
  // else
  // sysinb(port, data
  // end while

  while (1) {
    if (sys_inb(STAT_REG, data) != OK) {
      printf("Error: couldn't read the status register.");
      return 1;
    }

    if ( (*data & IBF) != 0)
     {
      tickdelay(micros_to_ticks(WAIT_KBC));
      continue;
    }
    else {
      sys_inb(port, data);
      return 0;
    }
  }
}

//interrupt handler
int (read_information)(uint8_t *data, uint8_t IRQ)
{
    uint32_t stat;

    //loads information from status register
    if(sys_inb_cnt(STAT_REG, &stat) != OK) {
      printf("Error: couldn't read the status register.");
      return 1;
    }
    //checks if information is available
    if( stat & OBF ) 
    {
      if(sys_inb_cnt(OUT_BUF, (uint32_t*) data) != OK) 
      {
        //loads information from output buffer to data 
        printf("Error: couldn't read the scancode from out buffer.");
        return 1;
      }
      //Checks the status for parity, timeout errors or if AUX is set
      if ((stat & (PAR_ERR | TO_ERR )) == 0 ) 
      {    
        if(IRQ == KEYBOARD_IRQ && ((AUX_SET&stat) == 0))
        return 0;
        else 
        if(IRQ == MOUSE_IRQ && ((AUX_SET&stat) != 0))
        return 0;
        
        else
        {
          printf("Error: Aux error occured");
          return 1;
        }
      }
      else
      {
        printf("Error: Parity | Timeout , error occured."); 
        return 1;
      }
    }
    else{
      printf("Error: No information available for reading"); 
     return 1; 
    }
  

}


//writes in kbc to a specified port
int (kbc_issue_command)(uint32_t cmd, int port)
{
  uint32_t stat;
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
      if(sys_outb(port, cmd) != OK) {
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


//write command byte to kbc
int (issue_cmd_b)(uint32_t cmdb)
{
  uint32_t stat;
  int n_tries = 0;
  //tries to issue the command responsible for sending the command byte
  if (kbc_issue_command(ISSUE_CMD_B,IN_BUF_CMD)!= OK)
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

//read command byte from kbc (only used for keyboard)
int (read_cmd_b)(uint32_t *cmdb)
{
  uint32_t stat;
  
  //tries to issue the command responsible for reading the command byte
  if (kbc_issue_command(READ_CMD_B,IN_BUF_CMD) != OK)
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

int read_information_pl(uint8_t *data,uint8_t IRQ)
{
  /*
  1. Reads information from status register
  2. Checks if there is information available in output buffer
        - if no information available -> waits and tries again 1.
  3. Loads information 
  4. Checks for errors
  */
  uint32_t data_32;
  uint32_t stat;
    
  while(1)  // loop while 8042 output buffer is empty 
  {
      //loads information from status register
        if(sys_inb_cnt(STAT_REG, &stat) != OK) {
        printf("Error: couldn't read the status register.");
        return 1;
      }
      //checks if information is available
       if( stat & OBF) 
       {
        if(sys_inb_cnt(OUT_BUF, &data_32) != OK) 
        {
        //loads information from output buffer to data 
        printf("Error: couldn't read the scancode from out buffer.");
        return 1;
        }

        *data = (uint8_t) data_32;
      //Checks the status for parity, timeout errors or if AUX is set
      if ((stat & (PAR_ERR | TO_ERR )) == 0 ) 
      {
        if((IRQ == KEYBOARD_IRQ) && ((AUX_SET&stat) == 0))
        {
        return 0;
        }
        else if((IRQ == MOUSE_IRQ) && ((AUX_SET&stat) != 0))
        {
        return 0;
        }
        else
        {
          printf("Error: Aux error occured");
          return 1;
        }

      }
      else
      {
        printf("Error: Parity | Timeout , error occured."); 
        return 1;
      }
      }
      else
      {
        //wait time before checking again if output buffer is full
        tickdelay(micros_to_ticks(WAIT_KBC)); 
      }
  }
}
