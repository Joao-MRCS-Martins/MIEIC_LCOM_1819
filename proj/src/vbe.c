#include <lcom/lcf.h>
#include <lcom/vbe.h>
#include <lcom/video_gr.h>
//#include <machine/int86.h>

#include "vbe.h"

//our own implementation of vbe_get_mode_info that collects information about the graphics mode
int (vbe_get_info_mode)(uint16_t mode, vbe_mode_info_t* vbe) {

  struct reg86u reg86;
  
  memset(&reg86, 0, sizeof(reg86));
  mmap_t  map;
  phys_bytes buf;
  
  if(lm_init(true) == NULL) {
    printf("Error: failed to initialize low memory.");
    return 1;
  }

  if(lm_alloc(sizeof(vbe_mode_info_t),&map) == NULL) {
    printf("Error: failed to allocate memory block.");
    return 1;
  }

  buf = map.phys;
  
  reg86.u.w.ax = RET_VBE_MODE_INFO;
  reg86.u.w.es = PB2BASE(buf);
  reg86.u.w.di = PB2OFF(buf);
  reg86.u.w.cx = mode;
  reg86.u.b.intno = VBE_INT; 

  if(sys_int86(&reg86) != OK)
  {
    printf("Error: Failed to do kernel call. \n");
    return 1;
  }
  else if(reg86.u.b.ah == FUNC_CALL_FAILED)
  {
    printf("Error: function call failed. \n");
    return 1;
  }
  else if(reg86.u.b.ah == FUNC_NOT_SUPPORT_HW)
  {
    printf("Error: function is not supported in current HW configuration. \n");
    return 1;
  }
  else if (reg86.u.b.ah  == FUNC_CALL_INVALID)
  {
    printf("Error: function is invalid in current video mode. \n");
    return 1;
  }
  
  *vbe = *(vbe_mode_info_t*) map.virt;



  lm_free(&map);
  
  return 0;
}

//this function changes the graphics mode to the one passed by the parameter
int (change_vbe_mode)(uint16_t mode) {
  
 
  struct reg86u rg;
  memset(&rg, 0, sizeof(rg));

  rg.u.b.intno = VBE_INT;
  rg.u.w.ax = SET_VBE_MODE;
  rg.u.w.bx = mode | BIT(14);

  if(sys_int86(&rg) != OK) {
    printf("Error: failed to set graphics mode. \n");
    return 1;
  }
  else if(rg.u.b.ah == FUNC_CALL_FAILED)
  {
    printf("Error: function call failed. \n");
    return 1;
  }
  else if(rg.u.b.ah == FUNC_NOT_SUPPORT_HW)
  {
    printf("Error: function is not supported in current HW configuration. \n");
    return 1;
  }
  else if (rg.u.b.ah  == FUNC_CALL_INVALID)
  {
    printf("Error: function is invalid in current video mode. \n");
    return 1;
  }

  return 0;
}
