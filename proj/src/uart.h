

#define SP1_IRQ   4
#define SP2_IRQ   3

#define SP1_BASE_ADDR   0x3F8
#define SP2_BASE_ADDR   0x2F8

#define SP_RBR    0 /* Receiver Holding Register */
#define SP_THR    0 /* Transmitter Hoolding Register */
#define SP_IER    1 /* Interrupt Enable Register */
#define SP_IIR    2 /* Interrupt Identification Register */
#define SP_FCR    2 /* FIFO Control Register */
#define SP_LCR    3 /* Line Control Register */
#define SP_LSR    5 /* Line Status Register */

#define SP_DLAB_DLL       0           /* LSB of DL Register */
#define SP_DLAB_DLM       1           /* MSB of DL Register */
#define SP_DL_VALUE(b)    115200/b    /* Value of DL Register (b is the bit rate) */

#define SP_IIR_INT    0x0E    /* Interrupt pending (1110) */
#define SP_IIR_RLS    0x06    /* Receiver Line Status (0110) */
#define SP_IIR_RDR    0x04    /* Received Data Ready (0100) */
#define SP_IIR_CTO    0x0C    /* Character Timeout (1100) */
#define SP_IIR_THR    0x02    /* Transmitter Holding Register Empty (0010) */
#define SP_IIR_FIFO   0xC0    /* Both FIFOs Enabled */

#define SP_IER_ENABLE_RDA     BIT(0)
#define SP_IER_ENABLE_THR     BIT(1)
#define SP_IER_ENABLE_RLS     BIT(2)      /* Signals Error in bits 1-4 LSR */
#define SP_IER_CONFIG        (SP_IER_ENABLE_RDA | SP_IER_ENABLE_THR | SP_IER_ENABLE_RLS)

#define SP_LCR_LENGHT_8     (BIT(1) | BIT(0))
#define SP_LCR_1_STOP_BIT           BIT(2)
#define SP_LCR_ODD_PARITY           BIT(3)
#define SP_LCR_BREAK_CONTROL        BIT(6)
#define SP_LCR_CONFIG              (SP_LCR_LENGHT_8 | SP_LCR_1_STOP_BIT | SP_LCR_ODD_PARITY)
#define SP_LCR_DLAB                 BIT(7)

#define SP_FCR_ENABLE_FIFO          BIT(0)
#define SP_FCR_CLEAR_RCVR           BIT(1)
#define SP_FCR_CLEAR_XMIT           BIT(2)
#define SP_FCR_TRIGGER_4            BIT(6)
#define SP_FCR_TRIGGER_8            BIT(7)
#define SP_FCR_TRIGGER_14           BIT(6) | BIT(7)
#define SP_FCR_CONFIG              (SP_FCR_ENABLE_FIFO | SP_FCR_CLEAR_RCVR | SP_FCR_CLEAR_XMIT | SP_FCR_TRIGGER_8)

#define SP_LSR_RD               BIT(0)  /* Data for receiving */
#define SP_LSR_OVERRUN_ERROR    BIT(1)  /* Overwritten character */
#define SP_LSR_PARITY_ERROR     BIT(2)  /* Parity Error on received character */
#define SP_LSR_FRAMING_ERROR    BIT(3)  /* Absence of Stop Bit on received character */
#define SP_LSR_BREAK_INT        BIT(4)  /* Serial Data Input Line hold low for too much time */
#define SP_LSR_THR_EMPTY        BIT(5)  /* Ready to transmit a new char */
#define SP_LSR_TR_EMPTY         BIT(6)  /* THR and Transmitter Shift Register are Empty */
#define SP_LSR_FIFO_ERRO        BIT(7)  /* One Error (parity, framing, break) on FIFO buffer */
/* Bits 0-4 and 7 are reset to 0 when LSR is read */
