#pragma once
#include <lcom/lcf.h>
#include <minix/sysutil.h>


#include "event.h"

#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#define sys_inb_cnt(p, q) sys_inb(p, q)
#endif


/**
 * @brief Subscribes interrupts from the keyboard
 * 
 * @param irq_bitmask - id of the bitmask
 * @return int - Returns 0 if sucessful and 1 if not
 */
int keyboard_subscribe_int(uint8_t *irq_bitmask);


/**
 * @brief Unsbscribes the keyboard interrupts
 * 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int keyboard_unsubscribe_int();

/**
 * @brief Reads the status byte. Returns it via argument status
 * 
 * @param status - Pointer to byte where the status is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int keyboard_read_status(uint8_t *status);

//
/**
 * @brief Reads the data from output buffer. Returns it via argument data
 * 
 * @param data - Pointer to byte where the buffer data is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int keyboard_read_buffer(uint8_t *data);


/**
 * @brief Checks the status byte flags and returns if the output buffer can be read
 * 
 * @return true - the data in the output buffer can be read
 * @return false - the data can't be read
 */
bool keyboard_check_status();


/**
 * @brief Writes a command to the kbc (does not read the return)
 * 
 * @param cmd - the command to be issued (see i8042 for some MACROS)
 * @param args - the argument the issued command should have (only used when for Write Command Byte command)
 * @return int 
 */
int keyboard_write_command(uint32_t cmd, uint32_t args);

/**
 * @brief Transforms the keyboard interrupt into a event
 * 
 * @param p - Where the event is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int kbd_event(event_keyboard * p);
