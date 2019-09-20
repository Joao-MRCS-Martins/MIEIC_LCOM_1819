#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define KEYBOARD_IRQ 1
#define MOUSE_IRQ 12

//KBC addresses
#define STAT_REG 0x64 //status register
#define IN_BUF_CMD 0x64 //write command
#define IN_BUF_ARG 0x60 //write arguments
#define OUT_BUF 0x60 //read scancodes return values from kbc command 

//Scancodes
#define TWO_B_CODE 0xe0 //first byte of two byte scancodes
#define BREAK_CODE BIT(7) //mask to check if is breakcode
#define ESC_BREAK 0x81 //scancode for ESC

#define WAIT_KBC 20000 //delay
#define MAX_TRIES 5 

//Status register bits
#define PAR_ERR BIT(7)  //mask for parity error
#define TO_ERR BIT(6) //mask for timeout error
#define AUX_SET BIT(5) //mask for mouse data
#define IBF BIT(1) //mask for IBF
#define OBF BIT(0) //mask for OBF

//KBC commands
#define READ_CMD_B 0x20  //adress for read command
#define ISSUE_CMD_B 0x60 //adress for writting command

//KBC command byte
#define EN_INT_KBD BIT(0) //Enables interrupts on OBF ,from keyboard
#define DIS_CMD_MOUSE BIT(5)
#define DIS_KEY BIT(4)
#define EN_INT_MOUSE BIT(1)

#define DIS_INT_KBD 0xFE
#define DIS_INT_MOUSE 0xFD

//////////////////////////////////Mouse/////////////////////////////////////////////////

#define FLAG_BYTE1 BIT(3)
#define LB BIT(0)
#define RB BIT(1)
#define MB BIT(2)
#define X_SIGN BIT(4)
#define Y_SIGN BIT(5)
#define X_OVF BIT(6)
#define Y_OVF BIT(7)


//MOUSE KBC COMMANDS
#define DIS_MOUSE 0xA7
#define EN_MOUSE 0xA8
#define CHECK_MOUSE_INT 0xA9

#define ISSUE_BY2MOUSE 0xD4
#define EN_DATA_REP 0xF4
#define DIS_DATA_REP 0XF5
#define STREAM_MODE 0xEA
#define REMOTE_MODE 0xF0
#define READ_DATA 0xEB

//errors
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

//


#endif
