#pragma once
#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup mouse mouse
 * @{
 *
 * Mouse interaction functions
 */



extern uint8_t data;


/**
 * @brief Subscribes interrupts from the keyboard
 * 
 * @param irq_bitmask id of the periferal bit mask
 * @return int - Returns 0 if sucessful, 1 if not
 */
int mouse_subscribe_int(uint8_t *irq_bitmask);


/**
 * @brief Unsbscribes the keyboard interrupts
 * 
 * @return int - Returns 0 if sucessful, 1 if not
 */
int mouse_unsubscribe_int();


/**
 * @brief Reads the status byte. Returns it via argument status
 * 
 * @param status - Where the status byte is returned
 * @return int - Returns 0 if sucessful, 1 if not
 */
int kbc_read_status(uint8_t *status);


/**
 * @brief Reads the data from output buffer. Returns it via argument data
 * 
 * @param data - Where the buffer content is returned
 * @return int - Returns 0 if sucessful, 1 if not
 */
int kbc_read_buffer(uint8_t *data);



/**
 * @brief Checks the status byte flags and returns:
 * 
 * @return true - the data in the output buffer can be read
 * @return false - the data can't be read
 */
bool kbc_check_out_buf();

 
/**
 * @brief Checks the status byte flags and returns:
 * 
 * @return true -the input buffer isn't full, writing is possible
 * @return false - can't write to input buffer
 */
bool kbc_check_in_buf();


/**
 * @brief Checks if it can send commands and write the command on CMD_REG 0x64
 * 
 * @param cmd - Command to be written
 * @return int - Returns 0 if sucessful, 1 if not
 */
int kbc_write_command(uint32_t cmd);




/**
 * @brief Checks if it can send the arguments and write the arguments on CMD_ARGS_REG 0x60
 * 
 * @param arg - Argument to be written
 * @return int - Returns 0 if sucessful, 1 if not
 */
int kbc_write_argument(uint32_t arg);


/**
 * @brief Checks if it can send commands and write the command on WRITE_B_MOUSE 0x20
 * 
 * @param cmd - The command to be written
 * @param resp - Where the mouse answer is returned
 * @return int - Returns 0 if sucessful, 1 if not
 */
int mouse_write_command(uint32_t cmd, uint8_t *resp);



/**
 * @brief Handles the process of writing commands and checking their Acknowledgement on kbc or the mouse
 * 
 * @param cmd - The command to be written
 * @param arg - The argument to be written with the command(if needed)
 * @return int - Returns 0 if sucessful, 1 if not
 */
int kbc_command_handler(uint32_t cmd, uint32_t arg);



/**
 * @brief process the array that contains the packet and prepares to print it
 * 
 * @param pk - packet to be returned
 * @param bytes - Array with the bytes recieved
 * @param size - Size of the array
 */
void mouse_parse_packet(struct packet *pk, uint8_t bytes[], uint8_t size);



/**
 * @brief Transforms mouse interrupts into events
 * 
 * @param pk - Mouse packet as input
 * @return struct mouse_ev* - Returns a mouse event
 */
struct mouse_ev* mouse_get_event(struct packet *pk);

/** @} end of mouse */
