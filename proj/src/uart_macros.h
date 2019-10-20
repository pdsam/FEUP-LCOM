#pragma once

/** @defgroup uartmacros uartmacros
 * @{
 *
 * Uart related macros
 */

#define BIT(n) (0x01 << (n))

// IRQ Line
#define UART_COM1_IRQ 4 /**< @brief UART COM1 IRQ Line  */
#define UART_COM2_IRQ 3 /**< @brief UART COM2 IRQ Line  */

// I/O ports
#define UART_COM1_BASE_ADDR 0x3F8
#define UART_COM2_BASE_ADDR 0x2F8

#define UART_RBR 0 // read   /**< @brief Receiver Buffer Register */
#define UART_THR 0 // write   /**< @brief Transmitter Holding Register */

#define UART_IER 1 /**< @brief Interrupt Enable Register */

#define UART_IIR 2 // read   /**< @brief Interrupt Identification Register */
#define UART_FCR 2 // write   /**< @brief FIFO Control Register */

#define UART_LCR 3 /**< @brief Line Control Register */
#define UART_MCR 4 /**< @brief Modem Control Register */
#define UART_LSR 5 /**< @brief Line Status Register */
#define UART_MSR 6 /**< @brief Modem Status Register */
#define UART_SR 7  /**< @brief Scratchpad Register */

#define UART_DLL 0 /**< @brief Divisor Latch LSB */
#define UART_DLM 1 /**< @brief Divisor Latch MSB */

// Line Status Register

#define UART_LSR_RCV_DATA BIT(0)
#define UART_LSR_OVR_ERROR BIT(1)
#define UART_LSR_PAR_ERROR BIT(2)
#define UART_LSR_FRM_ERROR BIT(3)
#define UART_LSR_BRK_INT BIT(4)
#define UART_LSR_THRE BIT(5)
#define UART_LSR_TER BIT(6)
#define UART_LSR_FIFO_ERROR BIT(7)

// interrupt enabler
#define UART_IER_RCVD_DATA_INT BIT(0)
#define UART_IER_THRE_INT BIT(1)
#define UART_IER_ERROR_INT BIT(2)
#define UART_IER_MODEM_STAT_INT BIT(3)

// Interrupt Identification Register
#define UART_IIR_NO_INT BIT(0)
#define UART_IIR_RLS (BIT(1) | BIT(2))
#define UART_IIR_RDA BIT(2)
#define UART_IIR_FIFO_C_TIMEOUT (BIT(2) | BIT(3))
#define UART_IIR_THRE BIT(1)
#define UART_INT_ID_MASK (BIT(1) | BIT(2) | BIT(3))

//divisor latch
#define UART_LCR_DLAB BIT(7)

#define UART_LCR_8BIT BIT(1) | BIT(0)

/** @} end of uartmacros */

