#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <lcom/video_gr.h>
//#include <machine/int86.h>

#include "videog.h"
#include "vbe.h"
#include "keyboard.h"

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */ 
static unsigned h_res; /* Horizontal resolution in pixels */ 
static unsigned v_res; /* Vertical resolution in pixels */ 
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */ 
static phys_bytes vram_base;
static unsigned int vram_size;

/* Components of colors in direct mode (used in pattern) */
static uint8_t bluesize;
static uint8_t greensize;
static uint8_t redsize;
static uint8_t bluefield;
static uint8_t greenfield;
static uint8_t redfield;

//function that initializes the graphics in the mode passed by parameter
void* (vg_init) (uint16_t mode)
{
  /*if(change_vbe_mode(mode) !=OK) {
    return NULL;
  }*/
  
  vbe_mode_info_t vbe_m_i;
  if(vbe_get_info_mode(mode,&vbe_m_i) != OK)
    return NULL;

  v_res= vbe_m_i.YResolution;
  h_res = vbe_m_i.XResolution;
  bits_per_pixel = vbe_m_i.BitsPerPixel;
  vram_base = (phys_bytes) vbe_m_i.PhysBasePtr;
  vram_size = h_res*v_res*BYTES_PER_PIXEL(bits_per_pixel);
  bluesize = vbe_m_i.BlueMaskSize;
  greensize = vbe_m_i.GreenMaskSize;
  redsize = vbe_m_i.RedMaskSize;
  bluefield = vbe_m_i.BlueFieldPosition;
  redfield = vbe_m_i.RedFieldPosition;
  greenfield = vbe_m_i.GreenFieldPosition;

  
  struct minix_mem_range mr;
  int r;

  /* Allow memory mapping */ 
  mr.mr_base = vram_base; 
  mr.mr_limit = mr.mr_base + vram_size; 

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    return NULL;
  }
  
  /* Map memory */ 
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size); 
  
  if(video_mem == MAP_FAILED) {
    panic("couldn't map video memory");
    return NULL;
  }

  if(change_vbe_mode(mode) !=OK) {
    return NULL;
  }

  
  return video_mem;
}

//function that will fill a pixel with the desired color, passed in the parameter
// at the coordinates passed (x,y)
//also passes an argument i that specifies the ratio bits_per_pixel
//auxiliary to test_rectangle and test_pattern
int vg_draw_pixel(unsigned int i,uint16_t x,uint16_t y,uint32_t color2)
{   
    //check if the pixel is within the screen borders
    if ( (x<0) || (x>=h_res) || (y<0) || (y>=v_res))
      return 1;
    
    for (uint8_t n = 0; n<i; n++)
    {
      //video_mem + y *h_res*i --> goes to beggining of the line to draw
      //video_mem + (y *h_res*i + x*i ) --> goes to the beggining of the position to draw in line
      //(video_mem + (y *h_res*i + x*i ) + n) --> goes to specific position in memory to draw in the line
        *(video_mem + (y *h_res*i + x*i ) + n) = color2;
        color2 = color2 >> 8;
    }
    return 0;
}


//function that will draw an horizontal line across the screen in the position
//specified by the parameters and color the pixels belonging with the color desired
//auxiliary to test_rectangle and test_pattern
int (vg_draw_hline)(uint16_t x,uint16_t y,uint16_t len, uint32_t color) 	
{
  
  uint32_t color2;
  uint16_t xfinal = x + len;

  unsigned int i = bits_per_pixel/8;

  //in case number is not even 
  if(bits_per_pixel % 8 != 0)
      i++;
  while(x < xfinal)
  { 
    color2 =  color;
    vg_draw_pixel(i,x,y,color2);
  
    x++;
  }
    
  return 0;
}

//function that will draw in a specified position a rectangle on (x,y) position
//with set width and height and colored with the value passes by argument
//auxiliary to test_rectangle and test_pattern
int (vg_draw_rectangle)(uint16_t	x,uint16_t y,uint16_t width,uint16_t height,uint32_t color)
{
  
  for(size_t i = 0; i < height; i++)
  {
    vg_draw_hline(x,y+i,width,color);
  }

  return 0;
}

//getter for v_res, for external use
uint16_t get_v_res()
{
  return v_res;
}
//getter for h_res, for external use
uint16_t get_h_res()
{
  return h_res;
}

//function that calculates a color by a given algorithm with specific parameters which are already
//passed on its call
//auxiliary to test_pattern
uint32_t color_mode(uint16_t row,uint16_t col, uint16_t mode, uint8_t step, uint32_t first,uint8_t no_rectangles)
{
  uint8_t R, G, B;
  uint8_t Rfirst, Gfirst, Bfirst;
  uint32_t mask;

  if( mode == INDEX)
  {
     return (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
  }
  else
  {
    /*
    Calculates components of first 
    - Component is shifted to lsb 
    - module of it with a (1 << component size) is always itself
    this way we can isolate each component of first
    */
    Rfirst  = first >> redfield % ( 1<< redsize);
    Gfirst  = first >> greenfield % ( 1<< greensize);
    Bfirst  = first >> bluefield % (1 <<bluesize);
    
     R = (Rfirst + col * step) % (1 << redsize);
     G = (Gfirst + row * step) % (1 << greensize);
     B = (Bfirst + (col + row) * step) % (1 << bluesize);
     
     /*
      Need to add all components in one single color
      Each component needs to be put in the respective bits (component << componentfield)
     */
     mask = (R << redfield ) + (G << greenfield ) + (B << bluefield);

     return mask;

  }
  
 return 0;
}

//cleans the screen by setting the video_memory, i.e. the screen,
//to black, which is the default "background"
void (clean_screen)() {
  memset(video_mem,0,h_res*v_res*bits_per_pixel/8);
}

//function that draws a given sprite on the screen
//using an object of the  already given class Sprite
void (draw_sprite)(Sprite* sp) {

  int yi= sp->y;
  int yf= sp->y + sp->height;
  int xi = sp->x;
  int xf = sp->x + sp->width;
  int counter=0;
  for(int y=yi; y < yf; y++)
  {
    for(int x=xi; x<xf ;x++) {
      vg_draw_pixel(1,x,y,sp->map[counter]);
      counter++;
    }
  }
}





























