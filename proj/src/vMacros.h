#ifndef _VIDEO_H_
#define _VIDEO_H_

/** @defgroup vMacros vMacros
 * @{
 *
 * video card related macros
 */

#define BIT(n) (0x01 << (n))

//BIOS

#define BIOS_VIDEO_SERVICE 0x10 // BIOS service interrupt vector for video card
#define BIOS_VBE_MODE 0x4F  // To diferenciate from the start BIOS video mode
#define MIX 0x4F02

//VBE function
#define VBE_RETURN_CONTROLER_INFO 0x00
#define VBE_RETURN_INFO 0x01
#define VBE_SET_MODE 0x02



//VBE function returns IN AL
#define FUNCTION_SUCESS_AH 0x00
#define FUNCTION_CALL_FAILED 0x01 //Function call failed
#define FUNCTION_NOT_SUPPORTED 0x02 //Function is not supported in current HW configuration
#define FUNCTION_INVALID 0x03 // Function is invalid in current video mode
#define FUNCTION_SUCESS 0x4F// value returned in AL register if the function is executed with sucess


//VBE MODES,  To be written in the BX register
#define VBE_MODE_105      0x105      //  VBE Graphics mode with 1024 x 765 pixels and 256 colors ;
#define VBE_MODE_115  0x115
#define VBE_SET_LINEAR_BUFFER      BIT(14) //  To be used along a VBE mode

#define SEGMENT(x) ((x)  >> 16)

#define OFFSET(x) (x & 0xFFFF)

#define TRON_BLUE 0x0144D6D

/** @} vMacros Penis */

#endif
