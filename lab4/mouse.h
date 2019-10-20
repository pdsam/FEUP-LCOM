#pragma once
#include <lcom/lcf.h>
#include <minix/sysutil.h>

extern uint8_t data;

//Subscribes interrupts from the keyboard
int mouse_subscribe_int(uint8_t *irq_bitmask);

//Unsbscribes the keyboard interrupts
int mouse_unsubscribe_int();

//Reads the status byte. Returns it via argument status
int kbc_read_status(uint8_t *status);

//Reads the data from output buffer. Returns it via argument data
int kbc_read_buffer(uint8_t *data);

//Checks the status byte flags and returns:
//  true -> the data in the output buffer can be read
//  false -> the data can't be read
bool kbc_check_out_buf();

//Checks the status byte flags and returns:
//  true -> the input buffer isn't full, writing is possible
//  false -> can't write to input buffer
bool kbc_check_in_buf();

//Checks if it can send commands and write the command on CMD_REG 0x64
//returns 0 when on sucess
//returns 1 when it fails
int kbc_write_command(uint32_t cmd);


//Checks if it can send the arguments and write the arguments on CMD_ARGS_REG 0x60
//returns 0 when on sucess
//returns 1 when it fails
int kbc_write_argument(uint32_t arg);

///Checks if it can send commands and write the command on WRITE_B_MOUSE 0x20
//returns 0 when on sucess
//returns 1 when it fails
int mouse_write_command(uint32_t cmd, uint8_t *resp);

//Handles the process of writing commands and checking their Acknowledgement on kbc or the mouse
//returns 0 when on sucess
//returns 1 when it fails
int kbc_command_handler(uint32_t cmd, uint32_t arg);


//process the array that contains the packet and prepares to print it
void mouse_parse_packet(struct packet *pk, uint8_t bytes[], uint8_t size);


//function used to debug the mouse status
//void kbc_debuger();
