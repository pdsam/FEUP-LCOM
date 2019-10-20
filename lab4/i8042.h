#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define BIT(n) (0x01 << (n)) /**< @brief Sets bit in position n as 1 */

#define KBD_IRQ 1         /**< @brief Keyboard IRQ line */
#define MOUSE_IRQ 12      /**< @brief Mouse IRQ line */
#define OUT_BUF 0x60      /**< @brief KBC output buffer port */
#define CMD_ARGS_REG 0x60 /**< @brief KBC command arguments register */
#define STATUS_REG 0x64   /**< @brief KBC status register */
#define CMD_REG 0x64      /**< @brief KBC commands register */

#define DELAY_US 20000 /**< @brief Delay until the Keyboard/Mouse responds to a command */

//Important keys
#define ESC_KEY 0x81            /**< @brief Esc key breakcode */
#define FIRST_OF_TWO_BYTES 0xE0 /** @brief First byte of two byte scan code */
//KBC Status register
#define KBC_PARE BIT(7)  /**< @brief Status register parity error */
#define KBC_TIMEO BIT(6) /**< @brief Status register timeout error */
#define KBC_AUXB BIT(5)  /**< @brief Status register mouse data */
#define KBC_KEYL BIT(4)  /**< @brief Status register keylock/inhibit flag */
#define KBC_CD BIT(3)    /**< @brief Status register command/A2 input line */
#define KBC_SYSF BIT(2)  /**< @brief Status register system flag */
#define KBC_INPB BIT(1)  /**< @brief Status register input buffer flag */
#define KBC_OUTB BIT(0)  /**< @brief Status register output buffer flag*/

//KBC Commands
#define READ_CMD_B 0x20  /**< @brief Returns command byte */
#define WRITE_CMD_B 0x60 /**< @brief Writes command byte (must pass command byte as argument to port 0x60) */
#define CHECK_KBC 0xAA   /**< @brief Self-test: returns 0x55 if OK, returns 0xFC if error */

//KBD Specific KBC Commands
#define CHECK_KB_IFC 0xAB /**< @brief Check Keyboard interface : returns 0 if OK */
#define DISABLE_KBD 0xAD  /**< @brief Disables Keyboard interface */
#define ENABLE_KBS 0xAE   /**< @brief Enables Keyboard interface */

//Mouse Specific KBC Commands
#define CHECK_MOUSE_IFC 0xA9 /**< @brief Check Mouse interface : returns 0 if OK */
#define DISABLE_MOUSE 0xA7   /**< @brief Disables Mouse interface */
#define ENABLE_MOUSE 0xA8    /**< @brief Enables Mouse interface */
#define WRITE_B_MOUSE 0xD4   /**< @brief Writes byte to mouse. The byte must be passed through CMD_ARGS_REG */

//Mouse Commands (Arguments of WRITE_B_MOUSE)
#define MOUSE_RESET 0xFF          /**< @brief Resets the mouse */
#define MOUSE_ENABLE_REPORT 0xF4  /**< @brief Enables Mouse data reporting*/
#define MOUSE_DISABLE_REPORT 0xF5 /**< @brief Disables Mouse data reporting*/
#define MOUSE_REMOTE_MODE 0xF0    /**< @brief Enable Mouse Remote Mode */
#define MOUSE_STREAM_MODE 0xEA    /**< @brief Enable Mouse Stream Mode */
#define MOUSE_READ_DATA 0xEB    /**< @brief Reads data from mouse */

//Mouse Controller command responses
#define ACK 0xFA   /**< @brief Acknowledged command or argument */
#define NACK 0xFE  /**< @brief Invalid byte */
#define ERROR 0xFC /**< @brief Second consecutive invalid byte */

//Mouse Packet Parsing
#define PK_LB BIT(0)  /**< @brief Left button flag */
#define PK_RB BIT(1)  /**< @brief Right button flag */
#define PK_MB BIT(2)  /**< @brief Middle button flag */
#define PK_BIT3 BIT(3)  /**< @brief Bit 3, always set on the first byte */
#define PK_XSIGN BIT(4)  /**< @brief X delta sign */
#define PK_YSIGN BIT(5)  /**< @brief Y delta sign */
#define PK_XOVF BIT(6)  /**< @brief X overflow flag */
#define PK_YOVF BIT(7)  /**< @brief Y overflow flag */

//Command Byte bits
#define ENABLE_KBD_IR BIT(0)   /**< @brief Enables Keyboard interrupts on OBF */
#define ENABLE_MOUSE_IR BIT(1) /**< @brief Enables Keyboard interrupts on OBF */

#endif
