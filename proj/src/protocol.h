#pragma once

/** @defgroup protocol protocol
 * @{
 *
 * Serial Port communication protocol
 */

#define PROT_ACK_BYTE 0xFA 
#define PROT_NACK_BYTE 0xFE
#define PROT_ERROR_BYTE 0xFC

#define PROT_HEADER 0x55
#define PROT_TAILER 0x88


#define PROT_CONNECT 0x58
#define PROT_UP_EVENT 0x01
#define PROT_DOWN_EVENT 0x02
#define PROT_RIGHT_EVENT 0x03
#define PROT_LEFT_EVENT 0x04
#define PROT_BOOST_EVENT 0x05
#define PROT_ROUND_START 0x06
// protocol settings

#define PROT_MAX_TRIES 3
#define PROT_DELAY 1000

/** @} end of protocol */
