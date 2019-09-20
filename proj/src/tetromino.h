#ifndef _TETROMINO_H_
#define _TETROMINO_H_

#include <lcom/lcf.h>
#include "videog.h"
#include "vbe.h"
#include "keyboard.h"
#include "kbc.h"

/** @defgroup tetramino tetramino
 * @{
 *
 * Functions for manipulating tetraminos and board
 */

/////////////////////////////////////
//  TETRAMINO and BOARD constants  //
/////////////////////////////////////
#define SIZE 20 //square size
#define SOUTH_BORDER 580
#define NORTH_BORDER 160
#define EAST_BORDER 550
#define WEST_BORDER 230
#define MOVE_DOWN true
#define MAX_BOARD 299


typedef struct {
  uint16_t x;               //position x on board
  uint16_t y;               //position y on board
  uint16_t color;
} Square;

typedef struct {
  Square positions[300];    //save information from board
  uint16_t width;           //size in squares of each line
  uint16_t height;          //size of squares of each collumn
  uint16_t x_gini;          //inicial position of board in the graphic position (left-top) in pixels
  uint16_t y_gini;          //inicial position of board in the graphic position (left-top) in pixels
} Board;

typedef struct {
  uint16_t sqidx[4];   //positions of squares in board
  uint16_t color;
  int rot;             //1 to 4
  Board *bd;
  int shape_n;         //1 to 7
} Tetramino;

/**
 * @brief Chooses the color of the piece according to its shape
 * 
 * @param shape_n - Identifies the piece shape
 * @return uint16_t - Return the color in 16-bit mode
 */
uint16_t choose_color(int shape_n);

/**
 * @brief Checks the board current status and if the player has reached to gameover state
 * 
 * @param board - Pointer to the board struct
 * @return true - If the player reached game over
 * @return false - If the player hasn't reached the end game
 */
bool check_for_gameover(Board *board);

/**
 * @brief Check the lines where a Tetramino lands to see if a line has been completed
 * 
 * @param t - Pointer to the tetramino struct
 * @param pos - Pointer to the array of positions
 */
void check_eliminate_line(Tetramino *t,int *pos);

/**
 * @brief Updates the board after a line has been removed
 * 
 * @param bd - Pointer to the board
 * @param position - Position of the first square on the eliminated line
 */
void update_board_after_line(Board *bd, int position);

/**
 * @brief Draws all squares of the board, painting it black the empty positions and non-black those who belong to tetraminos
 * 
 * @param bd - Pointer to the board
 */
void draw_squares(Board* bd);

/**
 * @brief Creates a new tetramino and loads initial information of the piece
 * 
 * @param board - Pointer to the board
 * @return Tetramino* - Returns a pointer to the Tetramino struct created 
 */
Tetramino* new_tetramino(Board* board);

/**
 * @brief Creates a new board and loads initial information
 * 
 * @return Board* - Returns  pointer to the Board struct created
 */
Board* new_board();

/**
 * @brief Deletes a board and frees the memory allocated to the struct
 * 
 * @param board - Pointer to the board to be deleted
 */
void delete_board(Board *board);

/**
 * @brief Deletes a tetramino and frees the memory alloacted to the struct
 * 
 * @param t - Pointer to the tetramino to be deleted
 */
void delete_tetramino(Tetramino* t);

#endif //_TETROMINO_H_
