#ifndef _KEYBOARD_H_ 
#define _KEYBOARD_H_

//Group added headers
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions to handle the keyboard
 */

/**
 * @brief Subscribe keyboard interruptions
 * 
 * @param bit_no - address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return int - Return 0 upon success and non-zero otherwise
 */
int keyb_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribe keyboard interruptions
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int keyboard_unsubscribe_int();

/**
 * @brief - Keyboard interrupt handler: collects information from keyboard
 * 
 */
void kbc_ih();


#endif // _KEYBOARD_H_
