#ifndef _VIDEOG_H_ 

#include "sprite.h"

//CONVERSIONS
#define BYTES_PER_PIXEL(x ) ((x+7)/8)

//useful macros
#define BLACK 0x00000000
#define INDEX 0x105
#define MAX_FR_RATE 60

//functions declaration
int (vg_draw_rectangle)(uint16_t	x,uint16_t y,uint16_t width,uint16_t height,uint32_t color);
uint16_t get_v_res();
uint16_t get_h_res();
uint32_t color_mode(uint16_t row,uint16_t col, uint16_t mode, uint8_t step, uint32_t first,uint8_t no_rectangles);
int vg_draw_pixel(unsigned int i,uint16_t x,uint16_t y,uint32_t color2);
void draw_sprite(Sprite* sp);
void clean_screen();


#endif // _VIDEOG_H_
