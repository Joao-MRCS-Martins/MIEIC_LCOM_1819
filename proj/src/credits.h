#ifndef _CREDITS_H_
#define _CREDITS_H_

#include "bitmap.h"
#include "videog.h"

/** @defgroup credits credits
 * @{
 *
 * Functions to handle the credits event
 */

typedef struct{
  Bitmap* image; //contains the credits image bitmap

} Graphic;

/**
 * @brief Presents Credits to user while ESC key isn't pressed
 * 
 * @return int - Returns 0 if all went as expected, returns 1 otherwise.
 */
int show_image();

#endif //_CREDITS_H_
