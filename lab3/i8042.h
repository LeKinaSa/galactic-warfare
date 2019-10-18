#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBD_IRQ   1     /* IRQ line for keyboard interrupts */

#define KBD_TIMEOUT_MAX_ATTEMPTS  5

/* PORTS */

#define KBD_STATUS_REG           0x64                 /* Status Register Port */
#define KBD_OUTPUT_BUF           0x60                 /* Output Buffer Port */
#define KBD_INPUT_COMMANDS_BUF   KBD_STATUS_REG       /* Command Port */
#define KBD_INPUT_ARGS_BUF       KBD_OUTPUT_BUF       /* Input arguments Port */


/* STATUS REGISTER */

#define KBD_PARITY_ERROR      BIT(7)    /* Bit set to 1 when there is a parity error */
#define KBD_TIMEOUT           BIT(6)    /* Bit set to 1 when keyboard timeout */
#define KBD_MOUSE_DATA        BIT(5)    /* Bit set to 1 if data comes from mouse */
#define KBD_INHINIB_FLAG      BIT(4)    /* Bit set to 1 if keyboard is inhibit */
#define KBD_A2                BIT(3)    /* Bit set to 1 if input is a command, 0 if input is data */
#define KBD_SYS               BIT(2)    /* Bit set to 1 if system initialized, 1 if in reset */
#define KBD_IN_BUF_FULL       BIT(1)    /* Bit set to 1 when input buffer is full */
#define KBD_OUT_BUF_FULL      BIT(0)    /* Bit set to 1 when output buffer is full */

/* KEYBOARD KBC COMMANDS */

#define KBD_READ_COMMAND_BYTE  0x20      /* Read command byte */
#define KBD_WRITE_COMMAND_BYTE 0x60      /* Write command byte */
#define KBD_CHECK_KBC          0xAA      /* Check KBC (self test) */
#define KBD_CHECK_INTERFACE    0xAB      /* Check KBC Interface */
#define KBD_DISABLE_INTERFACE  0xAD      /* Disable KBD Interface */
#define KBD_ENABLE_INTERFACE   0xAE      /* Enable KBD Interface */

/* SCANCODES */

#define KBD_BREAKCODE          BIT(7)    /* Bit set to 1 for breakcodes, 0 for makecodes */
#define KBD_TWO_BYTE_CODE      0xE0      /* Most significant byte for a two byte scancode */
#define KBD_ESC_BREAKCODE      0x81      /* ESC breakcode */

/* POLLING */

#define KBD_TIME_BETWEEN_POL   20000   /* Time in microseconds to wait between two polling cicles */

#endif /* _LCOM_I8042_H_ */
