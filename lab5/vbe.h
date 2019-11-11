#ifndef _LCOM_VGE_H_
#define _LCOM_VGE_H_

#include <lcom/lcf.h>

#define SERVICE_VIDEO_CARD      0x10

/* Graphics Mode */

#define MODE_GRAPHIC_INDEXED            0x105
#define MODE_DIRECT_SMALLER_RESOLUTION  0x110
#define MODE_DIRECT_COLOR_MODE          0x115
#define MODE_DIRECT_BIGGER_RESOLUTION   0x11A
#define MODE_DIRECT_32_BITS             0x14C
#define LINEAR_FRAME_BUFFER_MODEL       BIT(14)

/* Functions from VBE standard */

#define VBE_FUNCTION_AH                     0x4F

#define FUNC_RETURN_VBE_CONTROLLER_INFO     0x00
#define FUNC_RETURN_VBE_MODE_INFO           0x01
#define FUNC_SET_VBE_MODE                   0x02

#define RETURN_TO_TEXT_MODE_AH              0x00
#define RETURN_TO_TEXT_MODE_AL              0x03

#endif /* _LCOM_VGE_H_ */
