#include "serial_port.h"

static int serial_port_hook_id = SP1_IRQ;

int sp_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = serial_port_hook_id;

  if (sys_irqsetpolicy(SP1_IRQ, IRQ_REENABLE, &serial_port_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }
  
  return 0;
}

int sp_unsubscribe_int() {
  if (sys_irqrmpolicy(&serial_port_hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

int sp_config(uint32_t bit_rate) {
  /* Configure IER */
  if (sys_outb(SP1_BASE_ADDR + SP_IER, SP_IER_CONFIG)) {
    printf("Error when configuring Interrupt Enable Register.\n");
    return 1;
  }

  /* Configure LCR (to configure DL)*/
  if (sys_outb(SP1_BASE_ADDR + SP_LCR, SP_LCR_CONFIG | SP_LCR_DLAB)) {
    printf("Error when configuring Line Control Register.\n");
    return 1;
  }

  /* Configure DL Value */
  uint16_t dl_value = SP_DL_VALUE(bit_rate);
  uint8_t lsb = 0, msb = 0;
  util_get_LSB(dl_value, &lsb);
  util_get_MSB(dl_value, &msb);

  if (sys_outb(SP1_BASE_ADDR + SP_DLAB_DLL, lsb)) {
    printf("Error when writing LSB to DLL.\n");
    return 1;
  }
  if (sys_outb(SP1_BASE_ADDR + SP_DLAB_DLM, msb)) {
    printf("Error when writing MSB to DLL.\n");
    return 1;
  }

  /* Reconfigure LCR */
  if (sys_outb(SP1_BASE_ADDR + SP_LCR, SP_LCR_CONFIG)) {
    printf("Error when configuring Line Control Register.\n");
    return 1;
  }

  /* Configure FIFO */
  if (sys_outb(SP1_BASE_ADDR + SP_FCR, SP_FCR_CONFIG)) {
    printf("Error when configuring FIFO Control Register.\n");
    return 1;
  }

  return 0;
}

void sp_int_handler() {
  uint8_t iir = 0, error = 0;
  
  util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir);

  switch( iir & SP_IIR_INT ) {
      case SP_IIR_RDR:
        /* Read next char */
        break;
      case SP_IIR_THR:
        /* Transmit next char */
        break;
      case SP_IIR_CTO:
        /* Character Timeout */
        break;
      case SP_IIR_RLS:
        /* Receiver Line Status - Signals Error in LSR */
        util_sys_inb(SP1_BASE_ADDR + SP_LSR, &error);
        // WHAT DO I DO WITH THE ERROR ?????
        break;
      default:
        break;
  }
}

