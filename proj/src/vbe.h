#ifndef _VBE_H_
#define _VBE_H_

/** @defgroup vbe vbe
 * @{
 *
 * Functions for using the vbe driver
 */

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

/**
 * @brief Changes the video graphic mode
 * 
 * @param mode - Mode to be set
 * @return int  - Return 0 upon success and non-zero otherwise
 */
int change_vbe_mode(uint16_t mode);

/**
 * @brief Retrieves information about the video graphics memory and stores in a given struct
 * 
 * @param mode - Mode to receive information about
 * @param vbe_info - Pointer to the struct where the information will be stored
 * @return int - Return 0 upon success and non-zero otherwise 
 */
int vbe_get_info_mode(uint16_t mode, vbe_mode_info_t * vbe_info);

#endif //_VBE_H_
