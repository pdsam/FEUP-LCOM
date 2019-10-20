#include "serialPort.h"
#include "protocol.h"
#include "uart_macros.h"
#include <lcom/lcf.h>

static int uart_HookId = 4;

//used for syncronization
static uint8_t inBuffer;
//static uint8_t inBufferIndex = 0;
//static uint8_t *outBuffer;
//static uint8_t outBufferIndex = 0;

int uart_subscribe(uint8_t *irq_bitmask) {
  *irq_bitmask = BIT(uart_HookId);

  if (sys_irqsetpolicy(UART_COM1_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &uart_HookId) != OK) {
    printf("Error rtc subscribe\n");
    return 1;
  }

  return 0;
}

int uart_unsubscribe() {
  if (sys_irqrmpolicy(&uart_HookId) != OK) {
    printf("error rtc unsubcribing");
    return 1;
  }
  return 0;
}

int uart_readLSR(uint8_t *status) {
  uint32_t temp;
  if (sys_inb(UART_COM1_BASE_ADDR + UART_LSR, &temp) != OK) {
    printf("error reading LSR\n");
    return 1;
  }

  *status = (uint8_t) temp;
  return 0;
}

int uart_readRBR(uint8_t *buffer) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_RBR, &temp)) {
    printf("error reading RBR\n");
    return 1;
  }
  *buffer = (uint8_t) temp;
  return 0;
}

int uart_readIER(uint8_t *content) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_IER, &temp)) {
    printf("error reading IER\n");
    return 1;
  }
  *content = (uint8_t) temp;
  return 0;
}
//

int uart_readIIR(uint8_t *content) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_IIR, &temp)) {
    printf("error reading IIR\n");
    return 1;
  }
  *content = (uint8_t) temp;
  return 0;
}
int uart_readLCR(uint8_t *content) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_LCR, &temp)) {
    printf("error reading LCR\n");
    return 1;
  }
  *content = (uint8_t) temp;
  return 0;
}
int uart_readDLL(uint8_t *content) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_DLL, &temp)) {
    printf("error reading DLL\n");
    return 1;
  }
  *content = (uint8_t) temp;
  return 0;
}
int uart_readDLM(uint8_t *content) {
  uint32_t temp;

  if (sys_inb(UART_COM1_BASE_ADDR + UART_DLM, &temp)) {
    printf("error reading DLM\n");
    return 1;
  }
  *content = (uint8_t) temp;
  return 0;
}

int uart_write_THR(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_THR, data));
}
int uart_write_LCR(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_LCR, data));
}
int uart_write_IER(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_IER, data));
}
int uart_write_DLL(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_DLL, data));
}

int uart_write_DLM(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_DLM, data));
}
int uart_write_FCR(uint8_t data) {
  return (sys_outb(UART_COM1_BASE_ADDR + UART_FCR, data));
}

int uart_enable_divisor_latch() {
  u_int8_t temp;
  if (uart_readLCR(&temp) != OK)
    return 1;

  temp = (temp | UART_LCR_DLAB);

  return (uart_write_LCR(temp));
}
int uart_disable_divisor_latch() {
  u_int8_t temp;
  if (uart_readLCR(&temp) != OK)
    return 1;

  temp = temp ^ UART_LCR_DLAB;

  return (uart_write_LCR(temp));
}

int uart_set_bitrate(uint16_t bitrate) {

  uint16_t new_bitrate = 115200 / bitrate;

  uint8_t MSB_bitrate = (uint8_t)(new_bitrate & 0xff00) >> 8;
  uint8_t LSB_bitrate = (uint8_t)(new_bitrate & 0x00ff);

  if (uart_enable_divisor_latch() != OK) {
    printf("divisor latch enable error\n");
    return 1;
  }

  if (uart_write_DLL(LSB_bitrate) != OK) {
    printf("uart write dll error\n");
    return 1;
  }

  if (uart_write_DLM(MSB_bitrate) != OK) {
    printf("uart write dlm error\n");
    return 1;
  }

  if (uart_disable_divisor_latch() != OK) {
    printf("divisor latch disable error\n");
    return 1;
  }

  return 0;
}

int uart_write_character(uint8_t character) {
  int tries = 0;
  uint8_t status;
  while (tries < PROT_MAX_TRIES) {
    tries++;

    if (uart_readLSR(&status) != OK) {
      return 1;
    }

    if (status & UART_LSR_THRE) {
      return uart_write_THR(character);
    }
    else {
      tickdelay(micros_to_ticks(PROT_DELAY));
    }
  }

  return 1;
}

uint8_t uart_get_answer() {
  uint8_t content, answer_byte;
  uint8_t tries = 0;

  while (tries < PROT_MAX_TRIES) {
    tries++;
    if (uart_readLSR(&content) != OK) {
      return 1;
    }
    if (content & UART_LSR_RCV_DATA) {

      if (uart_readRBR(&answer_byte) != OK) {
        printf("error read RBR");
        return 1;
      }

      if (answer_byte == PROT_ACK_BYTE) {
        return answer_byte;
      }
      else if (answer_byte == PROT_NACK_BYTE) {
        return answer_byte;
      }
      else
        return PROT_ERROR_BYTE;
    }

    tickdelay(micros_to_ticks(PROT_DELAY));
  }

  return 1;
}

int uart_write_message(uint8_t content) {
  /*
  if (uart_write_caracter(PROT_HEADER) != OK) {
    return 1;
  }

  else if (uart_get_answer() != PROT_ACK_BYTE) {
    return 1;
  }
*/
  if (uart_write_character(content) != OK) {
    return 1;
  } /* else if (uart_get_answer() != PROT_ACK_BYTE) {
    return 1;
  }
*/
    /*
  if (uart_write_caracter(PROT_TAILER)) {
    return 1;
  } else if (uart_get_answer() != PROT_ACK_BYTE) {
    return 1;
  }
*/
  return 0;
}

int uart_init() {
  uart_set_bitrate(9600);
  uart_write_IER(UART_IER_ERROR_INT | UART_IER_RCVD_DATA_INT);
  uart_write_LCR(UART_LCR_8BIT);
  uart_write_FCR(BIT(2) | BIT(1) | BIT(0));
  //outBuffer = malloc(sizeof(uint8_t) * 16);
  return 0;
}

void uart_delete_buffers() {
  //free(inBuffer);
  //free(outBuffer);
}

int uart_read_data() {
  uint8_t lsr;
  uart_readLSR(&lsr);
  while (lsr & UART_LSR_RCV_DATA) {
    uint8_t data;
    uart_readRBR(&data);
    inBuffer = data;
    uart_readLSR(&lsr);
  }
  return 0;
}

void uart_ih() {
  uint8_t iir;
  uart_readIIR(&iir);
  uint8_t temp;
  if (!(iir & UART_IIR_NO_INT)) {
    switch (iir & UART_INT_ID_MASK) {
      case UART_IIR_RLS:
        uart_readLSR(&temp);
        //uart_write_message(PROT_NACK_BYTE);

        break;
      case UART_IIR_RDA:
        uart_read_data();
        break;
      case UART_IIR_FIFO_C_TIMEOUT:
        uart_read_data();
        break;
      case UART_IIR_THRE:
        break;

      default:
        break;
    }
  }
}

void uart_event(event_serial *p) {
  uart_ih();
  uint8_t temp = inBuffer;
  switch (temp) {
    case PROT_UP_EVENT:
      *p = up_key;
      break;
    case PROT_DOWN_EVENT:
      *p = down_key;
      break;
    case PROT_RIGHT_EVENT:
      *p = right_key;
      break;
    case PROT_LEFT_EVENT:
      *p = left_key;
      break;
    case PROT_BOOST_EVENT:
      *p = boost_key;
      break;
    case PROT_ROUND_START:
      *p = round_start;
      break;
    case PROT_CONNECT:
      *p = connection_byte;
      break;
    default:
      break;
  }
}
/*
int uart_connect(){
  uart_write_message(PROT_CONNECT);
  if( () == PROT_ACK_BYTE){
    printf("connected \n");
    return 0;
  }
  else return 1;
}
*/
