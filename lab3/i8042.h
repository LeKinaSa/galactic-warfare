#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBD_STATUS_REG           0x64                 /* Status Register Port */
#define KBD_OUTPUT_BUF           0x60                 /* Output Buffer Port */
#define KBD_INPUT_COMMANDS_BUF   KBD_STATUS_REG       /* Command Port */
#define KBD_INPUT_ARGS_BUF       KBD_OUTPUT_BUF       /* Input arguments Port */

//#define 

#define KBD_IN_BUFF_FULL      BIT(1)    /* Bit set when input buffer is full */
#define KBD_OUT_BUFF_FULL     BIT(0)    /* Bit set when output buffer is full */

#endif /* _LCOM_I8042_H_ */