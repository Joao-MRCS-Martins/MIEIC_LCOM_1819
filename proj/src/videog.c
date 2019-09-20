#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <lcom/video_gr.h>
//#include <machine/int86.h>

#include "videog.h"
#include "vbe.h"
#include "keyboard.h"

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */ 
static char *tmpBuffer;
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
  tmpBuffer = malloc(sizeof(char) * vram_size);
  
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
//implemented double buffering
int vg_draw_pixel(unsigned int i,uint16_t x,uint16_t y,uint32_t color2)
{   
    //check if the pixel is within the screen borders
    if ( (x<0) || (x>=h_res) || (y<0) || (y>=v_res))
      return 1;
    
    for (uint8_t n = 0; n<i; n++)
    {
      //tmpBuffer + y *h_res*i --> goes to beggining of the line to draw
      //tmpBuffer + (y *h_res*i + x*i ) --> goes to the beggining of the position to draw in line
      //(tmpBuffer + (y *h_res*i + x*i ) + n) --> goes to specific position in memory to draw in the line
        *(tmpBuffer + (y *h_res*i + x*i ) + n) = color2;
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

//function that will draw an vertical line across the screen in the position
//specified by the parameters and color the pixels belonging with the color desired
//developed for the project
int (vg_draw_vline)(uint16_t x,uint16_t y,uint16_t len, uint32_t color) 	
{
  
  uint32_t color2;
  uint16_t yfinal = y + len;

  unsigned int i = bits_per_pixel/8;

  //in case number is not even 
  if(bits_per_pixel % 8 != 0)
      i++;
  while(y < yfinal)
  { 
    color2 =  color;
    vg_draw_pixel(i,x,y,color2);
  
    y++;
  }
    
  return 0;
}

//function that will draw in a specified position an empty rectangle (only borders) on (x,y) position
//with set width and height and colored with the value passed by argument
//developed for the project
int (vg_draw_rect_empty)(uint16_t	x,uint16_t y,uint16_t width,uint16_t height,uint32_t color) {
  vg_draw_vline(x,y,height,color);
  vg_draw_vline(x+width,y,height,color);
  vg_draw_hline(x,y,width,color);
  vg_draw_hline(x,y+height,width,color);
  return 0;
}

//function that will draw in a specified position a rectangle on (x,y) position
//memwith set width and height and colored with the value passes by argument
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

char * get_mem()
{
  return video_mem;
}

char * get_tmp()
{
  return tmpBuffer;
}

unsigned get_vram_size() 
{
  return vram_size;
}

//cleans the screen by setting the video_memory, i.e. the screen,
//to black, which is the default "background"
void (clean_screen)() {
  memset(video_mem,0,h_res*v_res*bits_per_pixel/8);
}

void (clean_tmp)() {
  memset(tmpBuffer,0,h_res*v_res*bits_per_pixel/8);
}



