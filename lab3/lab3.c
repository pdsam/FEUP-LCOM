#include <i8042.h>
#include <keyboard.h>
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

//timer stuff
#include "i8254.h"
#include "timer.c"

//extern int kb_hookID;
extern uint32_t inb_counter;
extern uint8_t scancode;
extern unsigned int timer_interrupt_counter;
uint8_t size = 0;
uint8_t bytes[2];

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf functi  printf("wont stop");on calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)(bool assembly) {
  //irq_bitmask used to subscribe the interruption
  uint8_t irq_bitmask;

  if (keyboard_subscribe_int(&irq_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }

  //ESC_flag is true when the ESC breakcode is read from the buffer
  bool ESC_flag = false;
  int ipc_status;
  message msg;
  int r;

  //stops when the ESC breakcode is written in the console
  while (!ESC_flag) {
    // Get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_bitmask) { // subscribed interrupt
            if (assembly)
              kbc_asm_ih(); //assembly interrupt handler
            else
              kbc_ih(); //C interrupt handler

            //when the scancode is 0xE0, it is saved in the array and size is incremented
            //and the function moves on to read the second byte of the scancode
            if (scancode == FIRST_OF_TWO_BYTES) {
              bytes[0] = scancode;
              size++;
            }
            //when the scancode is not 0xE0 (either it is a single byte scancode or the second byte)
            //it is saved in the array (acording to previous size) and size is incremented
            //after that the scancode is printed and the size of the array is reset
            else {
              bool make;
              bytes[size] = scancode;
              size++;
              //when the last bit of the byte is set it is a breakcode, otherwise it is a makecode
              if ((BIT(7) & scancode) == BIT(7)) {
                make = false;
              }
              else {
                make = true;
              }
              kbd_print_scancode(make, size, bytes); //printing the scancode
              size = 0;                              //reseting size
            }

            //when the last scancode printed was the ESC breakcode, the flag is set in order to leave the loop
            if (bytes[0] == ESC_KEY)
              ESC_flag = true;
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

  //prints the number of sys_inb called when using the C interrupt handler
  if (!assembly)
    kbd_print_no_sysinb(inb_counter);

  if (keyboard_unsubscribe_int() != OK) {
    printf("kbd_test_scan::Error unsubscribing from the timer interrupt!\n");
    return 1;
  }

  return 0;
}

int(kbd_test_poll)() {
  //for pragmatic reasons we chose not to invoke kbd_ih on a polling function
  //for that reason, we don't use the global variable scancode this time and have a local scancd instead
  uint8_t scancd;
  bool ESC_flag = false;
  //stops when the ESC breakcode is written in the console
  do {

    //while there is no good data in the buffer the function will not move on
    while (!keyboard_check_status())
      continue;

    //reads the scancode from the buffer
    keyboard_read_buffer(&scancd);

    //works exactly the same as in kbd_test_scan
    if (scancd == FIRST_OF_TWO_BYTES) {
      bytes[0] = scancd;
      size++;
    }
    else {
      bool make;
      bytes[size] = scancd;
      size++;
      if ((BIT(7) & scancd) == BIT(7)) {
        make = false;
      }
      else {
        make = true;
      }
      kbd_print_scancode(make, size, bytes);
      size = 0; //reseting size
    }

    if (bytes[0] == ESC_KEY)
      ESC_flag = true;

  } while (!ESC_flag);

  //prints the number of sys_inb invoked for comparison with interrupts policy
  kbd_print_no_sysinb(inb_counter);

  //from now on, the function will reactivate the kbd interrupts
  uint8_t cmdReturn;

  //first issues the command READ COMMAND BYTE
  if (keyboard_write_command(READ_CMD_B, 0) != OK) {
    printf("kbd_test_poll::Error from keyboard__write_command\n");
    return 1;
  }

  //gives the kbc time to respond to the command
  tickdelay(micros_to_ticks(DELAY_US));

  //then reads the command byte from the buffer
  if (keyboard_read_buffer(&cmdReturn) != OK) {
    printf("kbd_test_poll::Error from keyboard_read_cmdReturn\n");
    return 1;
  }

  //preserves the previous command byte but enabling the interrupts
  uint32_t cmd_byte = cmdReturn | ENABLE_KBD_IR;

  //ends by writing the new command byte
  if (keyboard_write_command(WRITE_CMD_B, cmd_byte) != OK) {
    printf("kbd_test_poll::Error from keyboard_write_command\n");
    return 1;
  }

  return 0; //interrupts were activated and everything is back to normal
}

int(kbd_test_timed_scan)(uint8_t n) {
  //the two irq masks used for the subscriptions
  uint8_t irq_kbd, irq_timer0;

  //subscribing the keyboard interrupts
  if (keyboard_subscribe_int(&irq_kbd) != OK) {
    printf("kbd_test_timed_scan::Error subscribing keyboard");
    return 1;
  }

  //subscribing the timer interrupts
  if (timer_subscribe_int(&irq_timer0) != OK) {
    printf("kbd_test_timed_scan::Error subscribing timer0");
    return 1;
  }

  bool ESC_flag = false;
  int r;
  int ipc_status;
  message msg;

  //now, instead of just exiting when the ESC breakcode is written
  //the loop will also stop if there was no pressed key for (argument) n seconds
  while (!ESC_flag && (timer_interrupt_counter < (60 * n))) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_timer0) { /* subscribed timer interrupt */
            /* process timer0 interrupt request */
            //handler called to increment the global counter timer_interrupt_counter
            timer_int_handler();
          }
          if (msg.m_notify.interrupts & irq_kbd) { /* subscribed kbd interrupt */
            /* process kbd interrupt request */
            kbc_ih();

             //works exactly the same as in kbd_test_scan
            if (scancode == FIRST_OF_TWO_BYTES) {
              bytes[0] = scancode;
              size++;
            }
            else {
              bool make;
              bytes[size] = scancode;
              size++;
              if ((BIT(7) & scancode) == BIT(7)) {
                make = false;
              }
              else {
                make = true;
              }
              kbd_print_scancode(make, size, bytes);
              size = 0; //reseting size
            }

            if (bytes[0] == ESC_KEY)
              ESC_flag = true;

            timer_interrupt_counter = 0; //resets the counter to count the idle time starting from the last pressed key
          }
          break;

        default:
          break; /* no other notifications expected: do nothing */
      }
    }
  }

  //unsubscribes the kbd interrupts
  if (keyboard_unsubscribe_int() != OK) {
    printf("kbd_test_timed_scan::Error unsubscribing from the kbd interrupt!\n");
    return 1;
  }

  //unsbscribes the timer interrupts
  if (timer_unsubscribe_int() != OK) {
    printf("kbd_test_timed_scan::Error unsubscribing from the timer interrupt!\n");
    return 1;
  }

  return 0;
}
