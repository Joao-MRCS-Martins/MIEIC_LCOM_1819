#include "tetromino.h"
#include "lcom/lcf.h"
#include "videog.h"
#include "update_mov.h"

//choose the color for the shape
uint16_t(choose_color)(int nShape) {
  uint16_t pieceColor[7] = {RED, LIGHT_BLUE, GREEN, PINK, ORANGE, BLUE, YELLOW};
  return pieceColor[nShape - 1];
}

bool check_for_gameover(Board *board) {
  for (int i = 0; i < 15; i++) {
    if (board->positions[i].color != BLACK)
      return true;
  }

  return false;
}

void check_eliminate_line(Tetramino *t, int *pos) {

  int line, flag, position;
  flag = 0;

  line = t->sqidx[0] / 15; //line number

  position = line * 15  ;  //first position of the line

  for (int i = 0; i < 4 && position <= 299 ; i++) {
    int l = position +15;
    flag =0;

    for (int n = position; n < l; n++) {

      if (t->bd->positions[n].color == BLACK)
      {
        flag = 1;
      }        
    }

     if(flag != 1)
     pos[i] = position;

   position = position +15;
 
  }

}

void update_board_after_line(Board *bd, int position) 
{
  int line = position /15;
  int pos = position;

  for(int i = line; i > -1; i--)
  {
      int p = pos;
    for(int n = pos -15 ;n < pos; n++)
    {
      uint16_t color = bd->positions[n].color;
      bd->positions[p].color = color;
      p++;
    }
    pos -= 15;
  }

    for(int i = 0 ;i < 14;i++)
    {
      bd->positions[i].color = BLACK;
      pos++;
    }

}

void(draw_squares)(Board *bd) {

  //auxiliary to coversion from position in board to position on screen
  uint16_t xo, yo;
  yo = get_v_res() - ((bd->height + 1) * SIZE);
  xo = get_h_res() - (bd->width * SIZE);
  xo = xo >> 1;

  for (int i = 0; i < 300; i++) {
    //position in board to position on screen
    uint16_t xp, yp;
    xp = SIZE * bd->positions[i].x + xo;
    yp = SIZE * bd->positions[i].y + yo;
    vg_draw_rectangle(xp, yp, SIZE, SIZE, bd->positions[i].color);
  }
}

/**
 * Shape number:
 * 1- square shape (red)
 * 2- T shape (light blue)
 * 3- L shape (green)
 * 4- 7 shape inverted (pink)
 * 5- z shape inverted (orange)
 * 6- z shape (blue)
 * 7- | shape (yellow)
 * */
Tetramino *(new_tetramino)(Board *board) {

  Tetramino *tetra = malloc(sizeof(Tetramino));
  //randomly choose shape to be created: square, L, T,etc
  tetra->shape_n = rand() % 7 + 1;
  tetra->color = choose_color(tetra->shape_n);
  tetra->bd = board;
  tetra->rot =1;

  tetra->sqidx[0] = 7;

  if (tetra->shape_n == 2) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 9;
    tetra->sqidx[3] = 23;
  }
  else if (tetra->shape_n == 1) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 22;
    tetra->sqidx[3] = 23;
  }
  else if (tetra->shape_n == 3) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 9;
    tetra->sqidx[3] = 22;
  }
  else if (tetra->shape_n == 4) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 9;
    tetra->sqidx[3] = 24;
  }
  else if (tetra->shape_n == 5) {
    tetra->sqidx[1] = 22;
    tetra->sqidx[2] = 23;
    tetra->sqidx[3] = 38;
  }
  else if (tetra->shape_n == 6) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 23;
    tetra->sqidx[3] = 24;
  }
  else if (tetra->shape_n == 7) {
    tetra->sqidx[1] = 8;
    tetra->sqidx[2] = 9;
    tetra->sqidx[3] = 10;
  }

  for (size_t i = 0; i < 4; i++) {
    tetra->bd->positions[tetra->sqidx[i]].color = tetra->color;
  }
  return tetra;
}

Board *new_board() {
  Board *board = calloc(1, sizeof(Board));
  board->width = 15;
  board->height = 20;
  board->x_gini = 250;
  board->y_gini = 180;

  int n = 0;

  for (size_t i = 0; i < board->height; i++)
    for (size_t j = 0; j < board->width; j++) {
      board->positions[n].y = i;
      board->positions[n].x = j;
      board->positions[n].color = BLACK;
      n++;
    }

  return board;
}

void delete_board(Board *board) {
  if (board == NULL)
    return;

  free(board->positions);
  free(board);
}

void delete_tetramino(Tetramino *t) {
  if (t == NULL)
    return;

  free(t);
}
