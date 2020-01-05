#include "i8254.h"

#define SP1_IRQ   4     /* IRQ Line for Serial Port 1 Interrupts */
#define SP2_IRQ   3     /* IRQ Line for Serial Port 2 Interrupts */

#define SP1_BASE_ADDR   0x3F8       /* Serial Port 1 Base Address */
#define SP2_BASE_ADDR   0x2F8       /* Serial Port 2 Base Address */

#define SP_RBR    0 /* Receiver Holding Register Address Offset */
#define SP_THR    0 /* Transmitter Holding Register Address Offset */
#define SP_IER    1 /* Interrupt Enable Register Address Offset */
#define SP_IIR    2 /* Interrupt Identification Register Address Offset */
#define SP_FCR    2 /* FIFO Control Register Address Offset */
#define SP_LCR    3 /* Line Control Register Address Offset */
#define SP_LSR    5 /* Line Status Register Address Offset */

#define SP_DLAB_DLL       0           /* LSB of DL Register Address Offset */
#define SP_DLAB_DLM       1           /* MSB of DL Register Address Offset */
#define SP_DL_VALUE(b)    115200/b    /* Value of DL Register (b is the bit rate) */

#define SP_IIR_INT      0x0E    /* Interrupt pending (1110) */
#define SP_IIR_RLS      0x06    /* Receiver Line Status (0110) */
#define SP_IIR_RDR      0x04    /* Received Data Ready (0100) */
#define SP_IIR_CTO      0x0C    /* Character Timeout (1100) */
#define SP_IIR_THR      0x02    /* Transmitter Holding Register Empty (0010) */
#define SP_IIR_ERROR    0x0C    /* FIFO Timeout Error (1100) */
#define SP_IIR_FIFO     0xC0    /* Both FIFOs Enabled */

#define SP_IER_ENABLE_RDA     BIT(0)        /* Enable Receiver Interrupts */
#define SP_IER_ENABLE_THR     BIT(1)        /* Enable Transmitter Interrupts */
#define SP_IER_ENABLE_RLS     BIT(2)        /* Enable Error Interrupts -> Signals Error in bits 1-4 LSR */
#define SP_IER_CONFIG        (SP_IER_ENABLE_RDA | SP_IER_ENABLE_THR | SP_IER_ENABLE_RLS) /* IER Configuration */

#define SP_LCR_LENGHT_8            (BIT(1) | BIT(0))    /* Word Length 8 */
#define SP_LCR_1_STOP_BIT           BIT(2)              /* 1 Stop Bit */
#define SP_LCR_ODD_PARITY           BIT(3)              /* Odd Parity */
#define SP_LCR_BREAK_CONTROL        BIT(6)              /* Break Control */
#define SP_LCR_CONFIG              (SP_LCR_LENGHT_8 | SP_LCR_1_STOP_BIT | SP_LCR_ODD_PARITY)
#define SP_LCR_DLAB                 BIT(7)              /* Configure DLAB */

#define SP_DATA_LENGHT              8                   /* Word Length */
#define SP_STOP_BITS                1                   /* Stop Bits */

#define SP_FCR_ENABLE_FIFO          BIT(0)          /* Enable Fifo */
#define SP_FCR_CLEAR_RECEIVE        BIT(1)          /* Clear Receiver Information */
#define SP_FCR_CLEAR_TRANSMIT       BIT(2)          /* Clear Transmitter Information */
#define SP_FCR_TRIGGER_4            BIT(6)          /* FIFO Trigger 4 */
#define SP_FCR_TRIGGER_8            BIT(7)          /* FIFO Trigger 8 */
#define SP_FCR_TRIGGER_14           BIT(6) | BIT(7) /* FIFO Trigger 14 */
#define SP_FCR_CONFIG              (SP_FCR_ENABLE_FIFO | SP_FCR_CLEAR_RECEIVE | SP_FCR_CLEAR_TRANSMIT | SP_FCR_TRIGGER_4) /* LCR COnfiguration */

#define SP_FIFO_SIZE            16      /* FIFO Size */
#define SP_FIFO_TRIGGER         4       /* FIFO Trigger Level */

#define SP_BIT_RATE             9600    /* Bit Rate */

#define SP_INT_PER_TIMER_INT ((SP_BIT_RATE / SP_DATA_LENGHT) / SP_FIFO_TRIGGER) / TIMER0_INTERRUPTS_PER_SECOND /* Interrupts per Timer Interrupt */

/* Adjust the Trigger Level with the Bit Rate */
/* Bit Rate Standart Values : 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 */

#define SP_LSR_RD               BIT(0)  /* Data for receiving */
#define SP_LSR_OVERRUN_ERROR    BIT(1)  /* Overwritten character */
#define SP_LSR_PARITY_ERROR     BIT(2)  /* Parity Error on received character */
#define SP_LSR_FRAMING_ERROR    BIT(3)  /* Absence of Stop Bit on received character */
#define SP_LSR_BREAK_INT        BIT(4)  /* Serial Data Input Line hold low for too much time */
#define SP_LSR_THR_EMPTY        BIT(5)  /* Ready to transmit a new char */
#define SP_LSR_TR_EMPTY         BIT(6)  /* THR and Transmitter Shift Register are Empty */
#define SP_LSR_FIFO_ERROR       BIT(7)  /* One Error (parity, framing, break) on FIFO buffer */
/* Bits 0-4 and 7 are reset to 0 when LSR is read */

/* Utility Characters */

#define SP_RTC_INTERRUPT        'R'     /* Coordinates to PowerUp and Type of PowerUp */
#define SP_GENERATE_BULLET      'B'     /* Bullet Spawn */
#define SP_ENEMY_PLAYER         'P'     /* Coordinates and Angle for Enemy Player */
#define SP_NEW_SEQUENCE         'N'     /* Generate a new sequence (because of errors) */
#define SP_SEND_SEQUENCE        'S'     /* Ask to Send Sequence Again (because of errors) */
#define SP_END_SEQUENCE         'E'     /* Mark the End of a Sequence */

#define SP_PLAYER_SIZE           9      /* Player Packet Size */
#define SP_RTC_SIZE              6      /* RTC Packet Size */
#define SP_BULLET_SIZE           1      /* Bullet Packet Size */
#define SP_SEQUENCE_SIZE         SP_PLAYER_SIZE + SP_RTC_SIZE + SP_BULLET_SIZE + 2  /* Serial Port Trabsmission Max Size Without the Error Part */
