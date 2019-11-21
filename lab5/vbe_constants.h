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

#define VBE_RETURN_FUNC_SUPPORTED           0x4F
#define VBE_RETURN_SUCCESS                  0x00

#define FUNC_RETURN_VBE_CONTROLLER_INFO     0x00
#define FUNC_RETURN_VBE_MODE_INFO           0x01
#define FUNC_SET_VBE_MODE                   0x02

#define VBE_FUNC_AX(func)                   ((VBE_FUNCTION_AH << 8) | func)     /* AX register for a VBE function */
#define VBE_MODE_BX_LINEAR(mode)            (LINEAR_FRAME_BUFFER_MODEL | mode)  /* BX register for a VBE mode with linear frame buffer set */

/* Memory model */
#define VBE_PACKED_PIXEL      0x04    /* Indexed color */
#define VBE_DIRECT_COLOR      0x06    /* Direct color */

#define MIBIBYTE                            BIT(20)
#define BITS_PER_BYTE                       8         /* Number of bits in a byte */

#define VBE_INFO_BLOCK_SIZE     64      /* Bytes needed to allocate the buffer for VbeInfoBlock */

#define BITMASK(start, n_bits)              ((BIT(n_bits) - 1) << start)    /* Creates a bitmask starting at bit start with length n_bits */

#endif /* _LCOM_VGE_H_ */
