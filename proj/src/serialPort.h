#pragma once

#include <stdlib.h>
#include "event.h"
#include "protocol.h"

/** @defgroup serialPort serialPort
 * @{
 * Functions to interface with the seria port
 */

/**
 * @brief Enables Uart interrupts
 * 
 * @param irq_bitmask 
 * @return int 
 */
int uart_subscribe(uint8_t *irq_bitmask);

/**
 * @brief Disables Uart Interrupts
 * 
 * @return int- 0 if sucessful, 1 if not 
 */
int uart_unsubscribe();


//read functions

/**
 * @brief Reads the Line Status Register
 * 
 * @param status - The status byte read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readLSR(uint8_t * status);
/**
 * @brief Reads the Receiver Buffer Register
 * 
 * @param buffer - The content of the buffer 
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readRBR(uint8_t * buffer);
/**
 * @brief Reads the Interrupt Enable Register
 * 
 * @param content - The content read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readIER(uint8_t * content);
/**
 * @brief Reads the Interrupt Identification Register
 * 
 * @param content - The content read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readIIR(uint8_t * content);
/**
 * @brief Reads the Line Control Register
 * 
 * @param content - The content read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readLCR(uint8_t * content);
/**
 * @brief Reads the Divisor Latch LSB
 * 
 * @param content - The content read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readDLL(uint8_t * content);
/**
 * @brief Reads the Divisor Latch MSB
 * 
 * @param content - The content read
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_readDLM(uint8_t * content);

//writes funtions

/**
 * @brief Writes to Transmiter Holding Register
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_THR(uint8_t data);
/**
 * @brief Writes to Line Control Register
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_LCR(uint8_t data);
/**
 * @brief Writes to Interrupt Enable Register
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_IER(uint8_t data);
/**
 * @brief Writes to Divisor Latch LSB
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_DLL(uint8_t data);
/**
 * @brief Writes to Divisor Latch MSB
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_DLM(uint8_t data);
/**
 * @brief Writes to FIFO Control Register
 * 
 * @param data - The content to write in the register
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_FCR(uint8_t data);

//interaction with the overloaded registers

/**
 * @brief Enables the divisor latch
 * 
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_enable_divisor_latch();
/**
 * @brief Disables the divisor latch
 * 
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_disable_divisor_latch();

/**
 * @brief Changes Uart's bitrate
 * 
 * @param bitrate - value in Bits per seconds
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_set_bitrate(uint16_t bitrate);

/**
 * @brief Writes a character in the serial port to be transmited 
 * 
 * @param character - Character to send
 * @return int - Returns 0 if sucessful or 1 if not 
 */
int uart_write_character(uint8_t character);

/**
 * @brief Checks for acks and nacks 
 * 
 * @return u_int8_t - Returns either the acks or nacks, or 1 if neither acks or nacks were read  
 */
u_int8_t uart_get_answer();


/**
 * @brief writes to the buffer and checks for ACks and NAKS 
 * 
 * @param content - message to transmite
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_write_message(uint8_t content);


/**
 * @brief inicializes the uart with the project specific settings
 * 
 * @return int - Returns 0 if sucessful or 1 if not
 */
int uart_init();

/**
 * @brief Uart interrupt handler
 * 
 */
void uart_ih();
/**
 * @brief read data from the in fifo
 * 
 * @return int 
 */
int uart_read_data();

/**
 * @brief frees the alocated space buffers
 * 
 */
void uart_delete_buffers();
/**
 * @brief creates events to be handled by logic module
 * 
 * @param event_serial * - Where the event will be returned
 */
void uart_event(event_serial * p );


//int uart_connect();

/** @} end of serialPort */
