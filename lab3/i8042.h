#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBD_IRQ   1     /* IRQ line for keyboard interrupts */

/* PORTS */

#define KBD_STATUS_REG           0x64                 /* Status register port */
#define KBD_OUTPUT_BUF           0x60                 /* Output buffer port */
#define KBD_INPUT_COMMANDS_BUF   KBD_STATUS_REG       /* KBC command input port */
#define KBD_INPUT_ARGS_BUF       KBD_OUTPUT_BUF       /* Argument input port for KBC commands */


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

#define KBC_READ_COMMAND_BYTE  0x20      /* Read command byte */
#define KBC_WRITE_COMMAND_BYTE 0x60      /* Write command byte */
#define KBC_CHECK_SELF         0xAA      /* Check KBC (self test) */
#define KBC_CHECK_INTERFACE    0xAB      /* Check KBC Interface */
#define KBC_DISABLE_INTERFACE  0xAD      /* Disable KBD Interface */
#define KBC_ENABLE_INTERFACE   0xAE      /* Enable KBD Interface */

/* COMMAND BYTE */
#define CMD_BYTE_DISABLE_MOUSE      BIT(5)    /* Bit set to 1 if mouse is disabled */
#define CMD_BYTE_DISABLE_KBD        BIT(4)    /* Bit set to 1 if keyboard interface is disabled */
#define CMD_BYTE_ENABLE_MOUSE_INT   BIT(1)    /* Bit set to 1 if mouse interrupts are enabled */
#define CMD_BYTE_ENABLE_KBD_INT     BIT(0)    /* Bit set to 1 if keyboard interrupts are enabled */

/* SCANCODES */

#define KBD_BREAKCODE          BIT(7)    /* Bit set to 1 for breakcodes, 0 for makecodes */
#define KBD_TWO_BYTE_CODE      0xE0      /* Most significant byte for a two byte scancode */
#define KBD_ESC_BREAKCODE      0x81      /* ESC breakcode */

/* POLLING */

#define KBD_POLLING_INTERVAL   20000   /* Time in microseconds to wait between two polling cicles */


#define KBD_TIMEOUT_MAX_ATTEMPTS  5
#define KBC_COMMAND_WAIT_TIME     KBD_POLLING_INTERVAL * 50   /* Max wait time, in microseconds, when issuing a KBC command */

#endif /* _LCOM_I8042_H_ */
