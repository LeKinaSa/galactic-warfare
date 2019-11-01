#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/* Constants for programming the i8042 KBC */

#define KBD_IRQ     1     /* IRQ line for keyboard interrupts */
#define MOUSE_IRQ   12    /* IRQ line for mouse interrupts */

/* PORTS */

#define KBC_STATUS_REG           0x64                 /* Status register port */
#define KBC_OUTPUT_BUF           0x60                 /* Output buffer port */
#define KBC_INPUT_COMMANDS_BUF   KBC_STATUS_REG       /* KBC command input port */
#define KBC_INPUT_ARGS_BUF       KBC_OUTPUT_BUF       /* Argument input port for KBC commands */

/* STATUS REGISTER */

#define KBC_PARITY_ERROR      BIT(7)    /* Bit set to 1 when there is a parity error */
#define KBC_TIMEOUT           BIT(6)    /* Bit set to 1 when keyboard timeout */
#define KBC_MOUSE_DATA        BIT(5)    /* Bit set to 1 if data comes from mouse */
#define KBC_INHIBIT           BIT(4)    /* Bit set to 1 if keyboard is inhibited */
#define KBC_A2                BIT(3)    /* Bit set to 1 if input is a command, 0 if input is data */
#define KBC_SYS               BIT(2)    /* Bit set to 1 if system initialized, 1 if in reset */
#define KBC_IN_BUF_FULL       BIT(1)    /* Bit set to 1 when input buffer is full */
#define KBC_OUT_BUF_FULL      BIT(0)    /* Bit set to 1 when output buffer is full */

/* KBC COMMANDS */

#define KBC_READ_COMMAND_BYTE  0x20      /* Read command byte */
#define KBC_WRITE_COMMAND_BYTE 0x60      /* Write command byte */
#define KBC_CHECK_SELF         0xAA      /* Check KBC (self test) */
#define KBC_CHECK_INTERFACE    0xAB      /* Check KBC Interface */

/* COMMAND BYTE */

#define CMD_BYTE_DISABLE_MOUSE      BIT(5)    /* Bit set to 1 if mouse is disabled */
#define CMD_BYTE_DISABLE_KBD        BIT(4)    /* Bit set to 1 if keyboard interface is disabled */
#define CMD_BYTE_ENABLE_MOUSE_INT   BIT(1)    /* Bit set to 1 if mouse interrupts are enabled */
#define CMD_BYTE_ENABLE_KBD_INT     BIT(0)    /* Bit set to 1 if keyboard interrupts are enabled */

/* KEYBOARD-SPECIFIC KBC COMMANDS */

#define KBD_DISABLE_INTERFACE  0xAD      /* Disable keyboard interface */
#define KBD_ENABLE_INTERFACE   0xAE      /* Enable keyboard interface */

/* MOUSE-SPECIFIC KBC COMMANDS */

#define MOUSE_DISABLE          0xA7   /* Disable mouse */
#define MOUSE_ENABLE           0xA8   /* Enable mouse */
#define MOUSE_CHECK            0xA9   /* Check mouse interface */
#define MOUSE_WRITE_BYTE       0xD4   /* Write argument to mouse */

/* MOUSE COMMANDS */

#define MOUSE_RESET             0xFF  /* Mouse reset */
#define MOUSE_RESEND            0xFE  /* Resend the byte for serial communication errors */
#define MOUSE_SET_DEFAULT       0xF6  /* Set default values */
#define MOUSE_DISABLE_DATA      0xF5  /* In stream mode */
#define MOUSE_ENABLE_DATA       0xF4  /* Only in stream mode */
#define MOUSE_SET_SAMPLE_RATE   0xF3  /* Sets state sampling rate */
#define MOUSE_REMOTE_MODE       0xF0  /* Sets the mouse to Remote Mode, only send data on request */
#define MOUSE_READ_DATA         0xEB  /* Send data packet request */
#define MOUSE_STREAM_MODE       0xEA  /* Sets the mouse to Stream Mode, send data on events */
#define MOUSE_STATUS_REQUEST    0xE9  /* Gets mouse configuration (3 bytes) */
#define MOUSE_ACCELERATION_MODE 0xE7  /* Set scaling 2:1 acceleration mode */
#define MOUSE_LINEAR_MODE       0xE6  /* Set scaling 1:1 linear mode */

/* ACKNOLEDGMENT BYTES */

#define MOUSE_ACK_OK          0xFA    /* Everything OK */
#define MOUSE_ACK_INVALID     0xFE    /* Invalid byte */
#define MOUSE_ACK_ERROR       0xFC    /* Second consecutive invalid byte */

/* SCANCODES */

#define KBD_BREAKCODE          BIT(7)    /* Bit set to 1 for breakcodes, 0 for makecodes */
#define KBD_TWO_BYTE_CODE      0xE0      /* Most significant byte for a two byte scancode */
#define KBD_ESC_BREAKCODE      0x81      /* ESC breakcode */

/* MOUSE PACKET ASSEMBLING */

#define MOUSE_PCK_NUM_BYTES                 3
#define MOUSE_PCK_FIRST_BYTE                1
#define MOUSE_PCK_SECOND_BYTE               2
#define MOUSE_PCK_THIRD_BYTE                3
#define MOUSE_PACKET_START_OVER             0
#define MOUSE_INDEX_FIRST_BYTE              MOUSE_PCK_FIRST_BYTE - 1
#define MOUSE_INDEX_SECOND_BYTE             MOUSE_PCK_SECOND_BYTE - 1
#define MOUSE_INDEX_THIRD_BYTE              MOUSE_PCK_THIRD_BYTE - 1

/* MOUSE PACKET PARSING */

#define MOUSE_BYTE_TO_TREAT           MOUSE_INDEX_FIRST_BYTE
#define MOUSE_BYTE_X                  MOUSE_INDEX_SECOND_BYTE
#define MOUSE_BYTE_Y                  MOUSE_INDEX_THIRD_BYTE
#define MOUSE_OVERFLOW_Y              BIT(7)
#define MOUSE_OVERFLOW_X              BIT(6)
#define MOUSE_MSB_Y                   BIT(5)
#define MOUSE_MSB_X                   BIT(4)
#define MOUSE_FIRST_BYTE_CHECK        BIT(3)
#define MOUSE_MIDDLE_BUTTON_PRESSED   BIT(2)
#define MOUSE_RIGHT_BUTTON_PRESSED    BIT(1)
#define MOUSE_LEFT_BUTTON_PRESSED     BIT(0)

/* 9-BIT TO 16-BIT SIGN EXTENSION */

#define NEGATIVE_NUMBER               0xFF00
#define POSITIVE_NUMBER               0x0000

/* POLLING */

#define KBD_POLLING_INTERVAL   20000   /* Time in microseconds to wait between two polling cicles */

#define KBC_TIMEOUT_MAX_ATTEMPTS  5
#define KBC_COMMAND_WAIT_TIME     KBD_POLLING_INTERVAL * 25   /* Max wait time, in microseconds, when issuing a KBC command */
#define KBC_RETURN_VAL_WAIT_TIME  KBC_COMMAND_WAIT_TIME       /* Max wait time, in microseconds, when waiting for a return value to a KBC command */

#endif /* _LCOM_I8042_H_ */
