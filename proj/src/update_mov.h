#ifndef _UPDATE_MOV_H_
#define _UPDATE_MOV_H_

#include "tetromino.h"

/** @defgroup update_mov update_mov
 * @{
 *
 * Functions for update the tetraminos movement
 */

/**
 * @brief Update tetramino's movement if possible
 * 
 * @param t - tetramino to update movement
 * @param down - movement down case true
 * @param left - movement left case true else movement is rigth
 * @param rot  - rotation case true
 * @return true - update movement is succesful
 * @return false - update movement is not possible
 */
bool update_movement(Tetramino *t, bool down, bool left, bool rot);

/**
 * @brief Update tetramino's rotation if possible
 * 
 * @param t - tetramino to update rotation
 * @param n - array with new positions of tetramino agter rotation
 * @return true - rotation is succesful
 * @return false - rotation is not possible
 */
bool update_rotation(Tetramino *t,uint16_t *n);

#endif //_UPDATE_MOV_H_
