// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "mouse.h"
#include "stateMachine.h"

//timer0:
#include "i8254.h"
#include "timer.c"
//counter of the timer interrupts
extern unsigned int timer_interrupt_counter;

//used in interrupt loops
uint8_t size = 0; //size of the array of the packets
uint8_t bytes[3];//array with the bytes of the packets

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) {
  //irq_bitmask used to subscribe the interruption
  uint8_t irq_bitmask;

  //turning on date report
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_ENABLE_REPORT) != OK) {
    printf("mouse_test_packet::Error enabling data report\n");
    return 1;
  }

//subscribing
  if (mouse_subscribe_int(&irq_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }

//device handler variables
  unsigned int num_packets = 0;
  int ipc_status;
  message msg;
  int r;

  while (num_packets < cnt) {
    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_bitmask) { // subscribed interrupt
            mouse_ih();

            if (size == 0 && (data & PK_BIT3) == 0) {//checking if out of sync
              continue;
            }
            else {//if the array doesn't have all the bytes
              bytes[size] = data;
              size++;
            }

            if (size == 3) {//if the array contains all the bytes from a packet
              struct packet pk;
              mouse_parse_packet(&pk, bytes, size);
              mouse_print_packet(&pk);
              num_packets++;
              size = 0;
            }
          }
          break;

        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { //received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
//unsubscribing
  if (mouse_unsubscribe_int() != OK) {
    printf("mouse_test_packet::Error unsubscribing from the mouse interrupt!\n");
    return 1;
  }

  //from now on, the function will disable the mouse data reporting
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_DISABLE_REPORT) != OK) {
    printf("mouse_test_packet::Error disabling data report\n");
    return 1;
  }

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  unsigned int num_packets = 0;

  //turning on read data
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_READ_DATA) != OK) {
    printf("mouse_test_remote::Error requesting data from the mouse\n");
    return 1;
  }

  while (true) {
    while (!kbc_check_out_buf()) {//checks if valid data to read from the output
      continue;
    }

    kbc_read_buffer(&data);

    if (size == 0 && (data & PK_BIT3) == 0) {//checking if out of sync
      continue;
    }
    else {//if the packet array not full
      bytes[size] = data;
      size++;
    }

    if (size == 3) {//when the array has the full packet
      struct packet pk;
      mouse_parse_packet(&pk, bytes, size);
      mouse_print_packet(&pk);
      num_packets++;
      size = 0;

      //the loop stops when the number of packets displayed is the same as the user specified
      if (cnt == num_packets)
        break;

      //request another packet after period ms
      tickdelay(micros_to_ticks(period * 1000));
      if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_READ_DATA) != OK) {
        printf("mouse_test_remote::Error requesting data from the mouse\n");
        return 1;
      }
    }
  }
  //sets the streaming mode back on
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_STREAM_MODE) != 0) {
    printf("mouse_test_remote::Error reseting mouse to stream mode\n");
    return 1;
  }
  //disables data report mode
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_DISABLE_REPORT) != 0) {
    printf("mouse_test_remote::Error disabling data reporting\n");
    return 1;
  }

  uint32_t dflt_cmd_byte = (uint32_t) minix_get_dflt_kbc_cmd_byte();
  //updates to the default command byte from minix
  if (kbc_command_handler(WRITE_CMD_B, dflt_cmd_byte) != 0) {
    printf("mouse_test_remote::Error seting kbc command byte to defaulte\n");
    return 1;
  }

  return 0;
}

int(mouse_test_async)(uint8_t idle_time) {
  uint8_t irq_mouse;
  uint8_t irq_timer0;

//enabling data report
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_ENABLE_REPORT) != OK) {
    printf("mouse_test_async::Error enabling data report\n");
    return 1;
  }

//subscribing the mouse and the timer
  if (mouse_subscribe_int(&irq_mouse) != OK) {
    printf("mouse_test_async::Error subscribing mouse");
    return 1;
  }

  if (timer_subscribe_int(&irq_timer0) != 0) {
    printf("mouse_test_async::Error subscribing timer");
    return 1;
  }


  //loop variables
  int ipc_status;
  message msg;
  int r;
  int seconds = 0;
  int frequency = sys_hz();

  while (seconds != idle_time) {//checking if the time passed without packets is enough to stop the loop
    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_timer0) {
            //keeps track of the interrupts and converts them to seconds
            timer_int_handler();

            if (timer_interrupt_counter % frequency == 0) {

              seconds++;
            }
          }
          if (msg.m_notify.interrupts & irq_mouse) {
            //handles the mouse interrupt and reset the seconds and interrupt counters
            mouse_ih();
            timer_interrupt_counter = 0;
            seconds = 0;

            if (size == 0 && (data & PK_BIT3) == 0) {//checks if out of sync
              continue;
            }
            else {//if the array doesn't have out the bytes of the packet
              bytes[size] = data;
              size++;
            }

            if (size == 3) {//checks if the array has all the bytes
              struct packet pk;
              mouse_parse_packet(&pk, bytes, size);
              mouse_print_packet(&pk);
              size = 0;
            }
          }
          break;

        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { //received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
//unsubscribes the timer and the mouse
  if (timer_unsubscribe_int() != OK) {
    printf("mouse_test_async::Error unsubscribing from the mouse interrupt!\n");
    return 1;
  }

  if (mouse_unsubscribe_int() != OK) {
    printf("mouse_test_async::Error unsubscribing from the mouse interrupt!\n");
    return 1;
  }

  //from now on, the function will disable the mouse data reporting
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_DISABLE_REPORT) != OK) {
    printf("mouse_test_async::Error disabling data report\n");
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  //irq_bitmask used to subscribe the interruptions for the mouse
  uint8_t irq_bitmask;

  //enabling data report
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_ENABLE_REPORT) != OK) {
    printf("mouse_test_packet::Error enabling data report\n");
    return 1;
  }

  if (mouse_subscribe_int(&irq_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }
//interrupt loop variables
  int ipc_status;
  message msg;
  int r;
  struct packet pk;
  struct mouse_ev *evt;

  while (!gesture) {
    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_bitmask) { // subscribed interrupt
            mouse_ih();

            if (size == 0 && (data & PK_BIT3) == 0) {//checks if in sync
              continue;
            }
            else {//checks if the array doesn't contain all the bytes of the packet
              bytes[size] = data;
              size++;
            }

            if (size == 3) {//checks if the array has all the bytes from the packet
              mouse_parse_packet(&pk, bytes, size);
              mouse_print_packet(&pk);
              size = 0;

              evt = mouse_get_event(&pk);
              mouse_detect_gesture(evt, x_len, tolerance);
            }
          }
          break;

        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { //received a standard message, not a notification
      // no standard messages expected: do nothing
    }
  }
//unsubscribing the mouse
  if (mouse_unsubscribe_int() != OK) {
    printf("mouse_test_packet::Error unsubscribing from the mouse interrupt!\n");
    return 1;
  }

  //from now on, the function will disable the mouse data reporting
  if (kbc_command_handler(WRITE_B_MOUSE, MOUSE_DISABLE_REPORT) != OK) {
    printf("mouse_test_packet::Error disabling data report\n");
    return 1;
  }

  return 0;
}
