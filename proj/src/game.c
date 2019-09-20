
#include "lcom/lcf.h"
#include "lcom/pixmap.h"
#include "lcom/timer.h"
#include "lcom/vbe.h"

#include "game.h"
#include "kbc.h"
#include "keyboard.h"
#include "rtc.h"
#include "update_mov.h"

int ipc_status = 0;
message msg;
uint8_t r;

uint8_t irq_set_tim;
uint8_t irq_set_key;
uint8_t irq_set_rtc;

extern uint8_t data;
extern int ih_bool;
extern int _int_handler;
extern uint8_t sc_code[2];

extern int size;
extern bool make;

int score = 0;

Date d;

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void write_lvL(int level, ldig *ld) {

  char str[1];
  sprintf(str, "%d", level);
  char *t = concat("/home/lcom/labs/proj/resources/images/", str);
  char *final = concat(t, ".bmp");

  ld->image = loadBitmap(final);
  free(t);
  free(final);

  drawBitmap(ld->image, 700, 230, ALIGN_CENTER);
  deleteBitmap(ld->image);
}

void write_score(int pont, pnt *u, int x, int y) {

  int digits[5] = {0, 0, 0, 0, 0}; //used for line verification
  int n = 4;                       //number of digits unused in array diigits
  int i = 4;                       //control number of elements in array digits to be draw on screen
  int p = 0;                       //controls space between images

  if (pont == 0) { //score 0
    u->image = loadBitmap("/home/lcom/labs/proj/resources/images/0.bmp");
    drawBitmap(u->image, x, y, ALIGN_CENTER);
  }

  while (pont != 0) { //divide score in seperate digits
    digits[n] = pont % 10;
    pont = pont / 10;
    n--;
  }

  while (n != i) { //draw digits on screen

    char str[2];
    sprintf(str, "%d", digits[i]);
    char *t = concat( "/home/lcom/labs/proj/resources/images/",str);
    char *final = concat(t, ".bmp");

    u->image = loadBitmap(final);
    free(t);
    free(final);

    i--;
    drawBitmap(u->image, x - 30 * p, y, ALIGN_CENTER);
    p++;
  }

  deleteBitmap(u->image);
}

void draw_date_str(Date date, Date_image *di) {

  uint16_t x_init = 50;
  uint16_t y_init = 50;
  char *date_str = malloc(20 * sizeof(char));

  sprintf(date_str, "%02x/%02x/%02x %02x:%02x:%02x", date.day, date.month, date.year, date.hour, date.minutes, date.seconds);

  for (size_t i = 0; i < 17; i++) {
    
    if (date_str[i] == '/') {
      di->image = loadBitmap("/home/lcom/labs/proj/resources/images/slashp.bmp");
    }
    else if (date_str[i] == ':') {
      di->image = loadBitmap("/home/lcom/labs/proj/resources/images/_p.bmp");
    }
    else if (date_str[i] == ' ') {
      x_init += 15;
      continue;
    }
    else 
    {
    char o = date_str[i];
    char *number = &o;
    char *t = concat( "/home/lcom/labs/proj/resources/images/",number);
    char *final = concat(t, "p.bmp");

    di->image = loadBitmap(final);
    free(t);
    free(final);
    free(number);

    }

    drawBitmap(di->image, x_init, y_init, ALIGN_CENTER);
    x_init += 12;
  }

  deleteBitmap(di->image);
  free(date_str);
}

void paint_game() { //paint boards words score and level

  int y = 160;
  int x_bottom = 250;

  while (y < 600) {
    vg_draw_rectangle(WEST_BORDER, y, SIZE, SIZE, GREY);
    vg_draw_rectangle(EAST_BORDER, y, SIZE, SIZE, GREY);
    y += 20;
  }
  while (x_bottom < 550) {
    vg_draw_rectangle(x_bottom, SOUTH_BORDER, SIZE, SIZE, GREY);
    vg_draw_rectangle(x_bottom, NORTH_BORDER, SIZE, SIZE, GREY);
    x_bottom += 20;
  }

  //tetris image
  tetris *t = (tetris *) malloc(sizeof(tetris));
  t->image = loadBitmap("/home/lcom/labs/proj/resources/images/tetris.bmp");
  drawBitmap(t->image, TETRIS_IMG_X, 0, ALIGN_CENTER);
  deleteBitmap(t->image);

  //score image
  scoreimg *s = (scoreimg *) malloc(sizeof(scoreimg));
  s->image = loadBitmap("/home/lcom/labs/proj/resources/images/score.bmp");
  drawBitmap(s->image, 680, 40, ALIGN_CENTER);
  deleteBitmap(s->image);

  //level image
  lvl *l = (lvl *) malloc(sizeof(lvl));
  l->image = loadBitmap("/home/lcom/labs/proj/resources/images/lvl.bmp");
  drawBitmap(l->image, 700, 180, ALIGN_CENTER);
  deleteBitmap(l->image);
}

/*
Based on the makecode recieved chooses the actions to implement
-----makecode----
A - moves left 
D - moves right
S - moves down faster
Space - rotates
-----------------
Handles case when key is pressed for a long period of time 
(only want to act in the first makecode):
-counts number of makecodes between breakcodes
*/
void game_movement_options(uint32_t data, int *make_nbr, Tetramino *t) {

  if (data == AMAKE) { //move left
    if (*make_nbr == 0) {
      (*make_nbr)++;
      update_movement(t, false, true, false);
    }
    else
      (*make_nbr)++;
  }
  else if (data == DMAKE) { //move right
    if (*make_nbr == 0) {
      (*make_nbr)++;
      update_movement(t, false, false, false);
    }
    else
      (*make_nbr)++;
  }
  else if (data == SMAKE) { //move down faster
    if (*make_nbr == 0) {
      (*make_nbr)++;
      update_movement(t, true, false, false);
    }
    else {
      (*make_nbr)++;
      update_movement(t, true, false, false);
    }
  }
  else if (data == SPACEMAKE) { //rotacion
    if (*make_nbr == 0) {
      (*make_nbr)++;
      update_movement(t, false, false, true);
    }
    else
      (*make_nbr)++;
  }
  else if (data == ABREAK || data == DBREAK || data == SPACEBREAK || data == SBREAK) {
    *make_nbr = 0;
  }
}

uint8_t driver_Receive(endpoint_t any, message* m, int* ipc_status) {
  
  return driver_receive(any,m,ipc_status);
}

int game_main() {

  score = 0;
  bool gameover = false;
  int n = 0;        //used to count period of timer before leaving GameOver
  int points;       //used for level update
  int lvl_rate = 0; //used to change rate of movement based on level
  int make_nbr = 0;

  Board *board = new_board();
  Tetramino *newt = new_tetramino(board);

  //initialize score, level and data
  pnt *u = (pnt *) malloc(sizeof(pnt));
  ldig *ld = (ldig *) malloc(sizeof(ldig));
  Date_image *dimag = (Date_image *) malloc(sizeof(Date_image));

  //subscribe keyboard
  if (keyb_subscribe_int(&irq_set_key) != OK)
    return 1;

  //subscribe timer
  if (timer_subscribe_int(&irq_set_tim) != OK)
    return 1;

  //subscribe rtc
  if (rtc_subscribe_int(&irq_set_rtc) != OK)
    return 1;

  clean_screen(); //clean screen
  clean_tmp();    //clean back-buffer
  paint_game();
  write_lvL(1, ld);                               //draw initial level
  write_score(score, u, SCORE_X_POS, SCORE_Y_POS);                //draw initial score
  memcpy(get_mem(), get_tmp(), get_vram_size()); //draw in screen

  //Exit game if ESC is pressed
  while (data != ESC_BREAK) {

    if ((r = driver_Receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & BIT(irq_set_key)) { //handles keyboard interruptions

            kbc_ih();
            if (ih_bool != 0) //verify if handler is sucessful
              break;

            // process information from handler
            sc_code[size] = data;
            size++;
            make = !(data & BREAK_CODE);

            if (size == 2) { //game only processes one byte codes
              size = 0;
            }
            else if (size == 1) {
              if (data != TWO_B_CODE) { //handle next movement
                game_movement_options(data, &make_nbr, newt);
                size = 0;
              }
            }
            _int_handler = 0;
          }
          if (msg.m_notify.interrupts & BIT(irq_set_tim)) { //handles timer interruptions

            timer_int_handler(); //increase counter

            if (gameover) {
              if (_int_handler % 60 == 0) //counts period to leave game over
                n++;

              if (n == GAMEOVER_END) { //leave game after 3 seconds
                data = ESC_BREAK;
                delete_board(board);
                delete_tetramino(newt);
                delete_info(u,ld,dimag);
               
              }
               continue;
            }

            if (_int_handler % (RATE_LVL1 - lvl_rate) == 0) //rate movement based on level
            {
              if (!update_movement(newt, MOVE_DOWN, false, false)) {

                /*
                  auxiliary to CheckEliminateLine:
                  4 positions - one per line to verify full
                  each position is modify with the first square position of line in case is full
                */
                int pos[4] = {-1, -1, -1, -1};

                check_eliminate_line(newt, pos); //checks for full line.

                points = 0; //verify how many lines were full to update score

                if (pos[0] != -1) {
                  points++;
                  update_board_after_line(board, pos[0]);
                }
                if (pos[1] != -1) {
                  points++;
                  update_board_after_line(board, pos[1]);
                }
                if (pos[2] != -1) {
                  points++;
                  update_board_after_line(board, pos[2]);
                }
                if (pos[3] != -1) {
                  points++;
                  update_board_after_line(board, pos[3]);
                }

                //update score
                if (points == 1) {
                  score += POINTS_PER_LINE;
                }
                else if (points == 2) {
                  score += POINTS_PER_LINE_2;
                }
                else if (points == 3) {
                  score += POINTS_PER_LINE_3;
                }
                else if (points == 4) {
                  score += POINTS_PER_LINE_4;
                }

                //increments rate based on level
                if ((score > 90 && lvl_rate == 0) || (score > 190 && lvl_rate == 5)) {
                  lvl_rate += 5;
                }
                if ((score > 490 && lvl_rate == 10 ) || (score > 990 && lvl_rate == 13)) {
                  lvl_rate += 2;
                }

                //change level on screen
                if (lvl_rate == 5)
                  write_lvL(2, ld);
                else if (lvl_rate == 10)
                  write_lvL(3, ld);

                //update score on screen
                write_score(score, u,SCORE_X_POS,SCORE_Y_POS);

                if (check_for_gameover(board)) {

                  gameover = true;
                  clean_screen();
                  clean_tmp();

                  //draw gameover image
                  gameOver *g = (gameOver *) malloc(sizeof(gameOver));
                  g->image = loadBitmap("/home/lcom/labs/proj/resources/images/gameover.bmp");
                  drawBitmap(g->image, 0, 0, ALIGN_LEFT);

                  scoreimg *s = (scoreimg *) malloc(sizeof(scoreimg));
                  s->image = loadBitmap("/home/lcom/labs/proj/resources/images/finalscore.bmp");
                  drawBitmap(s->image, 400, 450, ALIGN_CENTER);
                  deleteBitmap(s->image);

                  write_score(score,u,SCORE_FINAL_X_POS,SCORE_FINAL_Y_POS);

                  memcpy(get_mem(), get_tmp(), get_vram_size());
                  deleteBitmap(g->image);

                  continue;
                }

                delete_tetramino(newt);
                newt = new_tetramino(board);
              }
            }

            //update screen
            paint_game();
            draw_squares(board);
            memcpy(get_mem(), get_tmp(), get_vram_size());
          }
          if (msg.m_notify.interrupts & BIT(irq_set_rtc)) { //handles rtc interruptions
            rtc_ih();
            d = get_date();
            draw_date_str(d, dimag);
          }

          break;
        default: // no other notifications expected: do nothing
          break;
      }
    }
    else {
      printf("Failed to receive notification from ipc. \n");
      return 1;
    }
  }

  if (timer_unsubscribe_int() != OK)
    return 1;

  if (keyboard_unsubscribe_int() != OK)
    return 1;

  if (rtc_unsubscribe_int() != OK)
    return 1;

  return 0;
}

void delete_info(pnt *u, ldig *ld, Date_image *dimag) {
  free(u);
  free(ld);
  free(dimag);
}
