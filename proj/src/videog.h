#ifndef _VIDEOG_H_ 
#define _VIDEOG_H_

/** @defgroup videog videog
 * @{
 *
 * Functions for using the video graphics
 */

//CONVERSIONS
#define BYTES_PER_PIXEL(x ) ((x+7)/8)

//useful macros
#define INDEX 0x105
#define DIRECT 0x114
#define MAX_FR_RATE 60

//colors (mode 0x114)
#define BLACK 0x0000
#define LIGHT_BLUE 0x05FF
#define GREEN 0x0F00
#define RED 0xF000
#define PINK 0xF0FF
#define YELLOW 0xFF00
#define GREY 0x7BEF
#define BLUE 0x00FF 
#define ORANGE 0xEC00
#define WHITE 0xFFFF

/**
 * @brief Paints a certain pixel with a given color. 
 * 
 * @param i - Ratio of bits/pixel
 * @param x - Coordinate x of pixel
 * @param y - Coordinate y of pixel
 * @param color2 - Color to paint the pixel
 * @return int - Return 0 upon success and non-zero otherwise
 */
int vg_draw_pixel(unsigned int i,uint16_t x,uint16_t y,uint32_t color2);

/**
 * @brief Draws a vertical line on the screem
 * 
 * @param x - Coordinate x of the line
 * @param y - Coordinate y of the line
 * @param len - Length of the line
 * @param color - Color of the line
 * @return int - Return 0 upon success and non-zero otherwise
 */
int vg_draw_vline(uint16_t x,uint16_t y,uint16_t len, uint32_t color);

/**
 * @brief Draws a rectangle composed only by its borders
 * 
 * @param x - Coordinate x of the rectangle
 * @param y - Coordinate y of the rectangle
 * @param width - Width of the rectangle
 * @param height - Height of the rectangle
 * @param color - Color of the rectangle
 * @return int - Return 0 upon success and non-zero otherwise
 */
int vg_draw_rect_empty(uint16_t	x,uint16_t y,uint16_t width,uint16_t height,uint32_t color);

/**
 * @brief Draws a rectangle on the screen
 * 
 * @param x - Coordinate x of the rectangle
 * @param y - Coordinate y of the rectangle
 * @param width - Width of the rectangle
 * @param height - Height of the rectangle
 * @param color - Color of the rectangle
 * @return int - Return 0 upon success and non-zero otherwise
 */
int vg_draw_rectangle(uint16_t	x,uint16_t y,uint16_t width,uint16_t height,uint32_t color);

/**
 * @brief Gets the vertical resolution
 * 
 * @return uint16_t - Returns the vertical resolution of the video graphics set
 */
uint16_t get_v_res();

/**
 * @brief Gets the horizontal resolution
 * 
 * @return uint16_t -  Returns the horizontal resolution of the video graphics set
 */
uint16_t get_h_res();

/**
 * @brief Gets the video memory buffer
 * 
 * @return char* - Returns a pointer to the video memory
 */
char * get_mem();

/**
 * @brief Gets the temporary video buffer
 * 
 * @return char* - Returns a pointer to the temporary buffer
 */
char * get_tmp();

/**
 * @brief Gets the vram size
 * 
 * @return unsigned - Returns the vram size
 */
unsigned get_vram_size();

/**
 * @brief Cleans the video memory, setting all its content to 0x0000 (Black)
 * 
 */
void clean_screen();

/**
 * @brief Cleans the temporary buffer, setting all its content to 0x0000 (Black)
 * 
 */
void clean_tmp();

#endif // _VIDEOG_H_
