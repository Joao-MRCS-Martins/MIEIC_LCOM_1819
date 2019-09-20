#include "update_mov.h"
#include <lcom/lcf.h>

bool(update_movement)(Tetramino *t, bool down, bool left, bool rot) {

  uint16_t newindx[4];

  uint16_t max = 299;

  if (rot) {
    if (!update_rotation(t, newindx))
      return false;
    else {
      for (size_t i = 0; i < 4; i++) {
        t->bd->positions[t->sqidx[i]].color = BLACK;
      }

      for (size_t i = 0; i < 4; i++) {
        t->bd->positions[newindx[i]].color = t->color;
        t->sqidx[i] = newindx[i];
      }

      return true;
    }
  }

  //----------------------------------down-------------------------------------------------
  if (down) {

    for (size_t i = 0; i < 4; i++) {
      newindx[i] = t->sqidx[i] + 15;
    }

    //first verify if the new position is within the game board
    if (newindx[3] > max)
      return false;

    //then verify for the specific piece if it meets the criteria
    else if ((t->shape_n == 2 && t->rot == 1) || (t->shape_n == 6 && (t->rot == 1 || t->rot == 3))) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 2) || (t->shape_n == 4 && t->rot == 4)) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 3) || (t->shape_n == 5 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 4) || (t->shape_n == 3 && t->rot == 4) || t->shape_n == 1 || (t->shape_n == 4 && t->rot == 2) || (t->shape_n == 6 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[3]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 3 && t->rot == 1) || (t->shape_n == 3 && t->rot == 3) || (t->shape_n == 4 && t->rot == 3)) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 3 && t->rot == 2) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 4 && t->rot == 1) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 5 && t->rot == 1) || (t->shape_n == 5 && t->rot == 3)) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 7 && (t->rot == 1 || t->rot == 3)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 7 && (t->rot == 2 || t->rot == 4)) {
      if (t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
  }
  //---------------------------------------------------------------------------------------

  //-------------------------------------right---------------------------------------------
  else if (!left) {
    for (size_t i = 0; i < 4; i++) {
      newindx[i] = t->sqidx[i] + 1;
    }

    //first verify if the new position is within the game board
    if ((t->shape_n == 2 && (t->rot == 1 || t->rot == 2 || t->rot == 4)) || (t->shape_n == 3 && (t->rot == 1 || t->rot == 2)) || (t->shape_n == 4 && t->rot == 1) || (t->shape_n == 6 && (t->rot == 2 || t->rot == 4))) {
      if (newindx[2] % 15 == 0)
        return false;
    }
    else if ((t->shape_n == 5 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 2 && t->rot == 3) || (t->shape_n == 3 && (t->rot == 3 || t->rot == 4)) || (t->shape_n == 6 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 7) || (t->shape_n == 1) || (t->shape_n == 4 && (t->rot == 2 || t->rot == 3))) {
      if (newindx[3] % 15 == 0)
        return false;
    }
    else if ((t->shape_n == 4 && t->rot == 4) || (t->shape_n == 5 && (t->rot == 2 || t->rot == 4))) {
      if (newindx[1] % 15 == 0)
        return false;
    }

    //then verify for the specific piece if it meets the criteria
    if (t->shape_n == 1 || (t->shape_n == 6 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 5 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 1) || (t->shape_n == 3 && t->rot == 1) || (t->shape_n == 4 && t->rot == 1)) {
      if (t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 5 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 6 && (t->rot == 2 || t->rot == 4)) || (t->shape_n == 2 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 3) || (t->shape_n == 3 && t->rot == 3) || (t->shape_n == 4 && t->rot == 3)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 3 && t->rot == 2) || (t->shape_n == 4 && t->rot == 4)) {
      if (t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 7 && (t->rot == 1 || t->rot == 3)) {
      if (t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 3 && t->rot == 4) || (t->shape_n == 4 && t->rot == 2)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 7 && (t->rot == 1 || t->rot == 3)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
  }
  //---------------------------------------------------------------------------------------

  //-----------------------------------left----------------------------------------------------
  else if (left) {
    for (size_t i = 0; i < 4; i++) {
      newindx[i] = t->sqidx[i] - 1;
    }

    //first verify if the new position is within the game board
    if ((t->shape_n == 2 && t->rot == 1) || (t->shape_n == 3 && (t->rot == 1 || t->rot == 2 || t->rot == 4)) || (t->shape_n == 4 && (t->rot == 1 || t->rot == 3 || t->rot == 4)) || (t->shape_n == 5 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 6 && (t->rot == 1 || t->rot == 3)) || t->shape_n == 7 || (t->shape_n == 1)) {
      if (newindx[0] % 15 == 14 || t->sqidx[0] == 0)
        return false;
    }
    else if ((t->shape_n == 2 && (t->rot == 3 || t->rot == 2 || t->rot == 4)) || (t->shape_n == 3 && t->rot == 3) || (t->shape_n == 6 && (t->rot == 2 || t->rot == 4))) {
      if (newindx[1] % 15 == 14  ||t->sqidx[1] == 15)
        return false;
    }
    else if ((t->shape_n == 4 && t->rot == 2)){
      if (newindx[2] % 15 == 14  ||t->sqidx[2] == 30 )
        return false;
    }
    else if(t->shape_n == 5 && (t->rot == 2 || t->rot == 4))
    {
       if (newindx[2] % 15 == 14  || t->sqidx[2] == 15 )
        return false;
    }

    //then verify for the specific piece if it meets the criteria
    if (t->shape_n == 1 || (t->shape_n == 6 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 4 && t->rot == 2) || (t->shape_n == 5 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 1) || (t->shape_n == 3 && t->rot == 1) || (t->shape_n == 4 && t->rot == 1)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 5 && (t->rot == 1 || t->rot == 3)) || (t->shape_n == 6 && (t->rot == 2 || t->rot == 4)) || (t->shape_n == 2 && (t->rot == 2 || t->rot == 4))) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 2 && t->rot == 3) || (t->shape_n == 3 && t->rot == 3) || (t->shape_n == 4 && t->rot == 3)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 3 && t->rot == 2) || (t->shape_n == 4 && t->rot == 4)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[3]].color != BLACK)
        return false;
    }
    else if ((t->shape_n == 3 && t->rot == 4)) {
      if (t->bd->positions[newindx[0]].color != BLACK || t->bd->positions[newindx[2]].color != BLACK || t->bd->positions[newindx[1]].color != BLACK)
        return false;
    }
    else if (t->shape_n == 7) {
      if (t->bd->positions[newindx[0]].color != BLACK)
        return false;
    }
  }
  //---------------------------------------------------------------------------------------

  for (size_t i = 0; i < 4; i++) {
    t->bd->positions[t->sqidx[i]].color = BLACK;
  }

  for (size_t i = 0; i < 4; i++) {
    t->bd->positions[newindx[i]].color = t->color;
    t->sqidx[i] = newindx[i];
  }

  return true;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

bool update_rotation(Tetramino *t, uint16_t *n) {

  //------------------------------------shape2--------------------------------------------
  if (t->shape_n == 2 && t->rot == 1) //1 to2
  {
    n[0] = t->sqidx[1] - 15;
    n[2] = t->sqidx[1];
    n[1] = t->sqidx[0];
    n[3] = t->sqidx[3];

    if (t->bd->positions[n[0]].color != BLACK || t->sqidx[1] < 15)
      return false;
  }
  if (t->shape_n == 2 && t->rot == 2) //2 to 3
  {
    n[3] = t->sqidx[2] + 1;
    n[0] = t->sqidx[0];
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[2];

    if (t->bd->positions[n[3]].color != BLACK || t->sqidx[2] % 15 == 14)
      return false;
  }
  if (t->shape_n == 2 && t->rot == 3) //3 to 4
  {
    n[0] = t->sqidx[0];
    n[1] = t->sqidx[2];
    n[2] = t->sqidx[3];
    n[3] = t->sqidx[2] + 15;

    if (t->bd->positions[n[3]].color != BLACK || n[3] > MAX_BOARD)
      return false;
  }
  if (t->shape_n == 2 && t->rot == 4) //4 to 1
  {
    n[0] = t->sqidx[1] - 1;
    n[2] = t->sqidx[2];
    n[1] = t->sqidx[1];
    n[3] = t->sqidx[3];

    if (t->bd->positions[n[0]].color != BLACK || t->sqidx[1] % 15 == 0)
      return false;
  }
  //---------------------------------------------------------------------------------------

  //------------------------------------shape3--------------------------------------------
  if (t->shape_n == 3 && t->rot == 1) //1 to2
  {
    n[0] = t->sqidx[1] - 16;
    n[2] = t->sqidx[1];
    n[1] = t->sqidx[1] - 15;
    n[3] = t->sqidx[1] + 15;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[1]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[0] < 15)
      return false;
  }
  if (t->shape_n == 3 && t->rot == 2) //2 to 3
  {
    n[0] = t->sqidx[1] + 1;
    n[1] = t->sqidx[2] - 1;
    n[2] = t->sqidx[2];
    n[3] = t->sqidx[2] + 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[1]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 14)
      return false;
  }
  if (t->shape_n == 3 && t->rot == 3) //3 to 4
  {
    n[0] = t->sqidx[0] - 1;
    n[1] = t->sqidx[2];
    n[2] = t->sqidx[2] + 15;
    n[3] = t->sqidx[2] + 16;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[2]].color != BLACK || t->bd->positions[n[3]].color != BLACK || n[2] > MAX_BOARD)
      return false;
  }
  if (t->shape_n == 3 && t->rot == 4) //4 to 1
  {
    n[0] = t->sqidx[1] - 1;
    n[2] = t->sqidx[1] + 1;
    n[1] = t->sqidx[1];
    n[3] = t->sqidx[2] - 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[2]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 0)
      return false;
  }
  //--------------------------------------------------------------------------------------

  //------------------------------------shape4--------------------------------------------
  if (t->shape_n == 4 && t->rot == 1) //1 to2
  {
    n[0] = t->sqidx[1] - 15;
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[0] + 15;
    n[3] = t->sqidx[1] + 15;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[2]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] < 15)
      return false;
  }
  if (t->shape_n == 4 && t->rot == 2) //2 to 3
  {
    n[0] = t->sqidx[0] - 1;
    n[1] = t->sqidx[1] - 1;
    n[2] = t->sqidx[1];
    n[3] = t->sqidx[1] + 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[1]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 14)
      return false;
  }
  if (t->shape_n == 4 && t->rot == 3) //3 to 4
  {
    n[0] = t->sqidx[0] + 1;
    n[1] = t->sqidx[0] + 2;
    n[2] = t->sqidx[2];
    n[3] = t->sqidx[2] + 15;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[1]].color != BLACK || t->bd->positions[n[3]].color != BLACK || n[3] > MAX_BOARD)
      return false;
  }
  if (t->shape_n == 4 && t->rot == 4) //4 to 1
  {
    n[0] = t->sqidx[2] - 1;
    n[2] = t->sqidx[2] + 1;
    n[1] = t->sqidx[2];
    n[3] = t->sqidx[3] + 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[2]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[0] % 15 == 0)
      return false;
  }
  //--------------------------------------------------------------------------------------

  //------------------------------------shape5--------------------------------------------
  if (t->shape_n == 5 && (t->rot == 1 || t->rot == 3)) //1 to2
  {
    n[0] = t->sqidx[1];
    n[1] = t->sqidx[2];
    n[2] = t->sqidx[1] + 14;
    n[3] = t->sqidx[1] + 15;

    if (t->bd->positions[n[2]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 0)
      return false;
  }
  if (t->shape_n == 5 && (t->rot == 2 || t->rot == 4)) //2 to 3
  {
    n[0] = t->sqidx[0] - 15;
    n[1] = t->sqidx[0];
    n[2] = t->sqidx[1];
    n[3] = t->sqidx[3] + 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[0] < 15)
      return false;
  }
  //--------------------------------------------------------------------------------------

  //------------------------------------shape6--------------------------------------------
  if (t->shape_n == 6 && (t->rot == 1 || t->rot == 3)) {
    n[0] = t->sqidx[1] + 1;
    n[1] = t->sqidx[2];
    n[2] = t->sqidx[3];
    n[3] = t->sqidx[2] + 15;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[3]].color != BLACK || n[3] > MAX_BOARD)
      return false;
  }
  if (t->shape_n == 6 && (t->rot == 2 || t->rot == 4)) {
    n[0] = t->sqidx[1] - 1;
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[3];
    n[3] = t->sqidx[3] + 1;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 0)
      return false;
  }
  //-------------------------------------------------------------------------------------

  //------------------------------------shape7--------------------------------------------
  if (t->shape_n == 7 && (t->rot == 1 || t->rot == 3)) {
    n[0] = t->sqidx[1] - 15;
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[1] + 15;
    n[3] = t->sqidx[1] + 30;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->bd->positions[n[2]].color != BLACK || n[3] > MAX_BOARD || t->sqidx[1] < 15)
      return false;
  }
  if (t->shape_n == 7 && (t->rot == 2 || t->rot == 4)) {
    n[0] = t->sqidx[1] - 1;
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[1] + 1;
    n[3] = t->sqidx[1] + 2;

    if (t->bd->positions[n[0]].color != BLACK || t->bd->positions[n[3]].color != BLACK || t->sqidx[1] % 15 == 0 || t->sqidx[1] % 15 == 14)
      return false;
  }
  //--------------------------------------------------------------------------------------

  //------------------------------------shape1--------------------------------------------
  if (t->shape_n == 1) {
    n[0] = t->sqidx[0];
    n[1] = t->sqidx[1];
    n[2] = t->sqidx[2];
    n[3] = t->sqidx[3];
  }
  //--------------------------------------------------------------------------------------

  if (t->rot == 4)
    t->rot = 1;
  else
    t->rot = t->rot + 1;

  return true;
}
