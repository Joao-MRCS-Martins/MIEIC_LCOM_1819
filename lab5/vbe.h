#ifndef _VBE_H_

#define BIT(n) (0x01<<(n))
//VBE functions
#define SET_VBE_MODE 0x4F02
#define RET_VBE_MODE_INFO 0x4F01
#define RET_VBE_CTRL_INFO 0x4F00
#define VBE_INT 0x10

//VBE return status
#define FUNC_CALL_FAILED 0x01
#define FUNC_NOT_SUPPORT_HW 0x02
#define FUNC_CALL_INVALID 0x03

//Functions for far pointers 
/*
 auxiliary to conversion to linear adress
 real mode adress --> segment:offset 
 linear adress --> segment << 4 + offset
 */

#define SEGMENT(x) ((x >> 16) << 4)
#define OFFSET(x) (x & 0xFFFF)


//function declaration
int change_vbe_mode(uint16_t mode);
int vbe_get_info_mode(uint16_t mode, vbe_mode_info_t * vbe_info);
int vbe_get_info_controller(vg_vbe_contr_info_t* send_info);


struct VBEInfoBlock_t {
  char VbeSignature[4];
  uint16_t VbeVersion;
  uint32_t *OemStringPtr;
  char Capabilities[4];
  uint32_t *VideoModePtr;
  uint16_t TotalMemory;
  uint16_t OemSoftwareRev;
  uint32_t *OemVendorNamePtr;
  uint32_t *OemProductNamePtr;
  uint32_t *OemProductRevPtr;
  char Reserved[222];
  char OemData[256];
}__attribute__ ((packed));

#endif //_VBE_H_
