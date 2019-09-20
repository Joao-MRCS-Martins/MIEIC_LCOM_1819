#ifndef _KBC_H_ 
#define _KBC_H_

/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the i8042 kbc
 */

//Group added headers
#include "i8042.h"
#include "i8254.h"

/* wrapper function*/
#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q) 
#endif

/**
 * @brief Tries to read information from the kbc status register
 * 
 * @param data - Pointer to the location where the data will be stored
 * @param port - The kbc port to be accessed
 * @return int - Returns 0 if successful, returns 1 otherwise
 */
int read_kbc(uint32_t *data, int port);

/**
 * @brief Reads information of the kbc, whether its the scancode from the keyboard or a packet from the mouse 
 * 
 * @param data - Pointer to the location where data is stored
 * @param IRQ - IRQ line to identify which periphal needs reading
 * @return int - Returns 0 if successful, returns 1 otherwise
 */
int read_information(uint8_t *data, uint8_t IRQ);

/**
 * @brief Issues a command to the kbc
 * 
 * @param cmd - the command to be issued
 * @param port - the port of the kbc to send the command
 * @return int - Returns 0 if successful, returns 1 otherwise
 */
int kbc_issue_command(uint32_t cmd, int port);

#endif // _KBC_H_
