#pragma once
#include <lcom/lcf.h>
#include <minix/sysutil.h>

#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#  define sys_inb_cnt(p, q) sys_inb(p, q)
#endif

//Subscribes interrupts from the keyboard
int keyboard_subscribe_int(uint8_t *irq_bitmask);

//Unsbscribes the keyboard interrupts
int keyboard_unsubscribe_int();

//Reads the status byte. Returns it via argument status
int keyboard_read_status(uint8_t *status);

//Reads the data from output buffer. Returns it via argument data
int keyboard_read_buffer(uint8_t *data);

//Checks the status byte flags and returns:
//  true -> the data in the output buffer can be read
//  false -> the data can't be read
bool keyboard_check_status();

//Writes a command to the kbc (does not read the return)
//cmd is the command to be issued (see i8042 for some MACROS)
//args is the argument the issued command should have (only used when for Write Command Byte command)
int keyboard_write_command(uint32_t cmd, uint32_t args);
