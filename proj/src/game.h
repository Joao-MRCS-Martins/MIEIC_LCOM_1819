#ifndef _GAME_H_
#define _GAME_H_

#include "bitmap.h"
#include "tetromino.h"
#include "rtc.h"

/** @defgroup game game
 * @{
 *
 * Functions for handling the game 
 */

////////////////////////
//    Keys Scancodes  //
////////////////////////
#define AMAKE 0x1E 
#define DMAKE 0x20 
#define SPACEMAKE 0x39 
#define SMAKE 0x1F
#define ABREAK 0x9E
#define DBREAK 0xA0
#define SPACEBREAK 0xB9
#define SBREAK 0x9F
#define ESC 0x81 

/////////////////////////////////
//    Game related constants	 //
/////////////////////////////////
#define GAMEOVER_END 3
#define POINTS_PER_LINE 10
#define POINTS_PER_LINE_2 30
#define POINTS_PER_LINE_3 40
#define POINTS_PER_LINE_4 60
#define RATE_LVL1 20
#define RATE_LVL2 15
#define RATE_LVL13 10
#define SCORE_Y_POS 100 
#define SCORE_X_POS 700
#define SCORE_FINAL_Y_POS 510 
#define SCORE_FINAL_X_POS 400
#define TETRIS_IMG_X 400
#define x_b_left 230
#define x_b_right 550

typedef struct{
  Bitmap* image; // contains game over image bitmap

} gameOver;

typedef struct{
  Bitmap* image; // contains tetris text image bitmap

} tetris;

typedef struct{
  Bitmap* image; // contains score image bitmap 

} pnt;

typedef struct{
  Bitmap* image; // contains score text image bitmap

} scoreimg;

typedef struct{
  Bitmap* image; // contains level text image bitmap

} lvl;

typedef struct{
  Bitmap* image; // contains level image bitmap

} ldig;

typedef struct {
  Bitmap* image; // contains date image bitmap

} Date_image;

/**
 * @brief Writes the level on the screen
 * 
 * @param level - Indicates which level the player is on (1,2 or 3)
 * @param l - Pointer to the struct in which is stored the bitmap of the level
 */
void write_lvL(int level, ldig *l);

/**
 * @brief Writes the score on the screen and in the game end
 * 
 * @param pont - The score obtained by the user
 * @param u - Pointer to the struct in which is stored the bitmap of the score
 * @param x - The x coordinate to place the score
 * @param y - The y coordinate to place the score
 */
void write_score(int pont, pnt *u, int x, int y);

/**
 * @brief Writes the date on the screen
 * 
 * @param d - Contains information about the date
 * @param di - Pointer to the struct in which is stored the bitmap of the date
 */
void draw_date_str(Date d, Date_image* di);

/**
 * @brief Draws on the screen the exteriors of the game: title, borders,score and level
 * 
 */
void paint_game();

/**
 * @brief Receives keyboard information and handles the event of the game accordingly
 * 
 * @param data - keyboard scancode
 * @param make_nbr - keeps track of the times a key is pressed to prevent unexpected movements
 * @param t - Pointer to the struct that contains the information about a tetramino
 */
void game_movement_options(uint32_t data, int *make_nbr,Tetramino *t);

/**
 * @brief Main function of the game, handles the flow of the game and presents the info to the player
 * 
 * @return int - Returns 0 if all went as expected, returns 1 otherwise
 */
int game_main();

/**
 * @brief Deletes the allocated memory, allowing to free memory space
 * 
 * @param u - Pointer to the struct where the score bitmap is stored
 * @param ld - Pointer to the struct where the level bitmap is stored
 * @param dimag - Pointer to the struct where the date bitmap is stored
 */
void delete_info(pnt *u, ldig *ld, Date_image *dimag);

/**
 * @brief Wrapper function for driver_receive. Used only for function call graph.
 * 
 * @param any 
 * @param m 
 * @param ipc_status 
 * @return uint8_t 
 */
uint8_t driver_Receive(endpoint_t any, message* m, int* ipc_status);

#endif //_GAME_H_
