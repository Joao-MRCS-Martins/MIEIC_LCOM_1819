#ifndef _MOUSE_H_ 
#define _MOUSE_H_

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the PS2 mouse
 */

//Group added headers
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "videog.h"

/**
 * @brief Subscribes mouse interruptions
 * 
 * @param bit_no - address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return int - Return 0 upon success and non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interruptions
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * @brief Issues a mouse related command to the keyboard
 * 
 * @param mcmd - Command to be issued
 * @return int - Return 0 if ACK byte is received, 1 if NACK byte is received and 2 if ERROR byte is received.
 */
int write_mouse_cmd(uint32_t mcmd);

/**
 * @brief Tries to issue mouse command while its not successful or an ERROR byte is received
 * 
 * @param mcmd - Command to be issued
 * @return int - Return 0 upon success and non-zero otherwise
 */
int try_mouse_cmd(uint32_t mcmd);

/**
 * @brief Mouse interrupt handler, reads mouse data from kbc 
 * 
 */
void mouse_ih();

/**
 * @brief Detects the mouse event by processing the packet passed in the parameter
 * 
 * @param pp - Mouse packet to be processed
 * @return struct mouse_ev* - Returns a pointer to a struct where the event information is stored
 */
struct mouse_ev* mouse_det_evt(struct packet * pp);

#endif // _MOUSE_H_
