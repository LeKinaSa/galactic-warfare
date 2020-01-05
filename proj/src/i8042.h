#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup i8042 i8042
 * @{
 * @brief Constants for programming the i8042 KBC 
 */

#define KBD_IRQ     1     /**< @brief IRQ line for keyboard interrupts */
#define MOUSE_IRQ   12    /**< @brief IRQ line for mouse interrupts */

/* PORTS */

#define KBC_STATUS_REG           0x64                 /**< @brief Status register port */
#define KBC_OUTPUT_BUF           0x60                 /**< @brief Output buffer port */
#define KBC_INPUT_COMMANDS_BUF   KBC_STATUS_REG       /**< @brief KBC command input port */
#define KBC_INPUT_ARGS_BUF       KBC_OUTPUT_BUF       /**< @brief Argument input port for KBC commands */

/* STATUS REGISTER */

#define KBC_PARITY_ERROR      BIT(7)    /**< @brief Bit set to 1 when there is a parity error */
#define KBC_TIMEOUT           BIT(6)    /**< @brief Bit set to 1 when a timeout occurred */
#define KBC_MOUSE_DATA        BIT(5)    /**< @brief Bit set to 1 if data comes from mouse */
#define KBC_INHIBIT_SWITCH    BIT(4)    /**< @brief Bit set to 0 if keyboard is inhibited */
#define KBC_A2                BIT(3)    /**< @brief Bit set to 1 if input is a command, 0 if input is data */
#define KBC_SYS               BIT(2)    /**< @brief Bit set to 1 if system initialized, 1 if in reset */
#define KBC_IN_BUF_FULL       BIT(1)    /**< @brief Bit set to 1 when input buffer is full */
#define KBC_OUT_BUF_FULL      BIT(0)    /**< @brief Bit set to 1 when output buffer is full */

/* KBC COMMANDS */

#define KBC_READ_COMMAND_BYTE  0x20      /**< @brief Read command byte */
#define KBC_WRITE_COMMAND_BYTE 0x60      /**< @brief Write command byte */
#define KBC_CHECK_SELF         0xAA      /**< @brief Check KBC (self test) */
#define KBC_CHECK_INTERFACE    0xAB      /**< @brief Check KBC Interface */

/* COMMAND BYTE */

#define CMD_BYTE_DISABLE_MOUSE      BIT(5)    /**< @brief Bit set to 1 if mouse is disabled */
#define CMD_BYTE_DISABLE_KBD        BIT(4)    /**< @brief Bit set to 1 if keyboard interface is disabled */
#define CMD_BYTE_ENABLE_MOUSE_INT   BIT(1)    /**< @brief Bit set to 1 if mouse interrupts are enabled */
#define CMD_BYTE_ENABLE_KBD_INT     BIT(0)    /**< @brief Bit set to 1 if keyboard interrupts are enabled */

/* KEYBOARD-SPECIFIC KBC COMMANDS */

#define KBC_DISABLE_KBD_INTERFACE  0xAD    /**< @brief Disable keyboard interface */
#define KBC_ENABLE_KBD_INTERFACE   0xAE    /**< @brief Enable keyboard interface */

/* MOUSE-SPECIFIC KBC COMMANDS */

#define KBC_DISABLE_MOUSE         0xA7   /**< @brief Disable mouse */
#define KBC_ENABLE_MOUSE          0xA8   /**< @brief Enable mouse */
#define KBC_CHECK_MOUSE           0xA9   /**< @brief Check mouse interface */
#define KBC_WRITE_BYTE_TO_MOUSE   0xD4   /**< @brief Write byte to mouse */

/* PS/2 MOUSE COMMANDS */

#define MOUSE_RESET             0xFF  /**< @brief Mouse reset */
#define MOUSE_RESEND            0xFE  /**< @brief Resend the byte for serial communication errors */
#define MOUSE_SET_DEFAULTS      0xF6  /**< @brief Set default values */
#define MOUSE_DISABLE_DATA      0xF5  /**< @brief Disables data reporting */
#define MOUSE_ENABLE_DATA       0xF4  /**< @brief Enables data reporting in stream mode */
#define MOUSE_SET_SAMPLE_RATE   0xF3  /**< @brief Sets state sampling rate */
#define MOUSE_REMOTE_MODE       0xF0  /**< @brief Sets the mouse to remote mode, only send data on request */
#define MOUSE_READ_DATA         0xEB  /**< @brief Send data packet request */
#define MOUSE_STREAM_MODE       0xEA  /**< @brief Sets the mouse to stream mode, send data on events */
#define MOUSE_STATUS_REQUEST    0xE9  /**< @brief Gets mouse configuration (3 bytes) */
#define MOUSE_ACCELERATION_MODE 0xE7  /**< @brief Set scaling 2:1 acceleration mode */
#define MOUSE_LINEAR_MODE       0xE6  /**< @brief Set scaling 1:1 linear mode */

/* ACKNOWLEDGMENT BYTES */

#define MOUSE_ACK_OK            0xFA     /**< @brief Everything OK */
#define MOUSE_ACK_INVALID       0xFE     /**< @brief Invalid byte */
#define MOUSE_ACK_ERROR         0xFC     /**< @brief Second consecutive invalid byte */

/* SCANCODES */

#define KBD_BREAKCODE           BIT(7)    /**< @brief Bit set to 1 for breakcodes, 0 for makecodes */
#define KBD_TWO_BYTE_CODE       0xE0      /**< @brief Most significant byte for a two byte scancode */
#define KBD_ESC_BREAKCODE       0x81

#define KBD_W_MAKECODE          0x11
#define KBD_W_BREAKCODE         0x91
#define KBD_A_MAKECODE          0x1E
#define KBD_A_BREAKCODE         0x9E
#define KBD_S_MAKECODE          0x1F
#define KBD_S_BREAKCODE         0x9F
#define KBD_D_MAKECODE          0x20
#define KBD_D_BREAKCODE         0xA0

/* MOUSE PACKET ASSEMBLY */

#define MOUSE_PCK_NUM_BYTES                 3   /**< @brief Number of bytes in a mouse packet */
#define MOUSE_INDEX_FIRST_BYTE              0   /**< @brief Index of the first byte in the packet */
#define MOUSE_INDEX_SECOND_BYTE             1   /**< @brief Index of the second byte in the packet */
#define MOUSE_INDEX_THIRD_BYTE              2   /**< @brief Index of the third byte in the packet */

/* MOUSE PACKET PARSING */

#define MOUSE_BYTE_INFO               MOUSE_INDEX_FIRST_BYTE    /**< @brief Index of the info byte in the packet */ 
#define MOUSE_BYTE_X                  MOUSE_INDEX_SECOND_BYTE   /**< @brief Index of the x-displacement byte in the packet */
#define MOUSE_BYTE_Y                  MOUSE_INDEX_THIRD_BYTE    /**< @brief Index of the y-displacement byte in the packet */
#define MOUSE_OVERFLOW_Y              BIT(7)                    /**< @brief Set to 1 on y-displacement overflow */
#define MOUSE_OVERFLOW_X              BIT(6)                    /**< @brief Set to 1 on x-displacement overflow */
#define MOUSE_MSB_Y                   BIT(5)                    /**< @brief Most significant bit of y-displacement */
#define MOUSE_MSB_X                   BIT(4)                    /**< @brief Most significant bit of x-displacement */
#define MOUSE_FIRST_BYTE_CHECK        BIT(3)                    /**< @brief Bit always set to 1 in first packet byte */
#define MOUSE_MIDDLE_BUTTON_PRESSED   BIT(2)                    /**< @brief Set to 1 if middle mouse button is pressed */
#define MOUSE_RIGHT_BUTTON_PRESSED    BIT(1)                    /**< @brief Set to 1 if right mouse button was pressed */
#define MOUSE_LEFT_BUTTON_PRESSED     BIT(0)                    /**< @brief Set to 1 if left mouse button was pressed */

/* 9-BIT TO 16-BIT SIGN EXTENSION */

#define NEGATIVE_NUMBER               0xFF00    /**< @brief Used in 9-bit to 16-bit sign extension of a negative number */
#define POSITIVE_NUMBER               0x0000    /**< @brief Used in 9-bit to 16-bit sign extension of a positive number */

/* POLLING */

#define KBD_POLLING_INTERVAL  20000   /**< @brief Time in microseconds to wait between two polling cicles */
#define MILI_TO_MICRO         1000    /**< @brief Number of microseconds in a milisecond */

/* ERROR HANDLING */

#define KBC_TIMEOUT_MAX_ATTEMPTS      5                         /**< @brief Max attempts upon a timeout error */
#define KBC_COMMAND_WAIT_TIME         KBD_POLLING_INTERVAL * 10 /**< @brief Max wait time, in microseconds, when issuing a KBC command */
#define KBC_RETURN_VAL_WAIT_TIME      KBC_COMMAND_WAIT_TIME     /**< @brief Max wait time, in microseconds, when waiting for a return value to a KBC command */
#define MOUSE_COMMAND_MAX_ATTEMPTS    3                         /**< @brief Max attempts when sending a command to the PS/2 mouse */

/**@}*/

#endif /* _LCOM_I8042_H_ */
