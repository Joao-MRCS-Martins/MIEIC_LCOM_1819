#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_


#include "bitmap.h"
#include "videog.h"
#include "tetromino.h" //for cursor color 

/** @defgroup main_menu main_menu
 * @{
 *
 * Functions for handle the main menu events
 */

//OPTIONS
#define MAIN_MENU 0
#define GAME 1
#define CREDITS 2
#define EXIT 3

//buttons borders
//Play button
#define PLAY_X 330
#define PLAY_WIDTH 150
#define PLAY_Y 250
#define PLAY_HEIGHT 45

//Credits button
#define CREDITS_X 330
#define CREDITS_WIDTH 150
#define CREDITS_Y 300
#define CREDITS_HEIGHT 55

//Quit button
#define QUIT_X 330
#define QUIT_WIDTH 150
#define QUIT_Y 380
#define QUIT_HEIGHT 45

#define CURSOR_SPEED 1.2 //Cursor speed multiplier
#define CURSOR_SIZE 20 //Cursor size (square)


typedef struct{
  Bitmap* image; // contains the background image bitmap

} Background;

/**
 * @brief Handles mouse interruptions 
 * 
 * @param pp1 - packet of information from mouse
 * @param irq_set_mouse - 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int handling_interruptions(struct packet *pp1,uint8_t irq_set_mouse);

/**
 * @brief Draw mouse pointer on screen
 * 
 * @param x - horizontal position of mouse on screen
 * @param y - vertical position of mouse on screen
 * @param color - color to fill the pointer
 */
void draw_pointer(uint16_t x, uint16_t y,uint16_t color);

/**
 * @brief Tracks the mouse related events, presenting to the user the main menu of the game
 * 
 * @return int - Returns 0 upon sucess and non-zero otherwise
 */
int detect_options_mouse();

#endif //_MAIN_MENU_H_
