
//#include <lcom/keyboard.h>
//#include <lcom/lab3.h>

#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"
#include "keyboard.h"

//global variables
int kb_hookID = 1;        //hook_id used for the keyboard
uint32_t inb_counter = 0; //global sys_inb counter
uint8_t scancode;         //global variable to pass the scancodes from the output buffer to lab3.c

//Modified version of sys_inb. This one also counts the number of calls
int(sys_inb_cnt)(port_t port, uint32_t *byte) {
  inb_counter++;
  return sys_inb(port, byte);
}

int(keyboard_subscribe_int)(uint8_t *irq_bitmask) {
  //irq_bitmask is a return argument which contains the bit mask created from hook_id
  *irq_bitmask = BIT(kb_hookID);

  //subscribes notifications from keyboard based on hook_id passed as 3rd argument
  //from now on the IRQ line 1 will be masked
  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_hookID) != OK) {
    printf("sys_irqsetpolicy::Error setting policy\n");
    return 1;
  }

  //Everything went as expected
  return 0;
}

int(keyboard_unsubscribe_int)() {
  //Removes the subscription from the mask created with kb_hookID
  //IRQ line 1 will be unmasked
  if (sys_irqrmpolicy(&kb_hookID) != OK) {
    printf("keyboard_unsubscribe_int::Error unsubscribing the previous subscription of the interrupt notification on the IRQ line!\n");
    return 1;
  }

  //Everything went as expected
  return 0;
}

int(keyboard_read_status)(uint8_t *status) {
  //temporary variable to save what's read from the status register
  uint32_t temp;

  if (sys_inb_cnt(STATUS_REG, &temp) != OK) {
    printf("keyboard_read_status::Error using sys_inb_cnt\n");
    return 1;
  }

  //status is a return argument which contains the status byte
  *status = (uint8_t) temp;
  return 0;
}

int(keyboard_read_buffer)(uint8_t *data) {
  //temporary variable to save what's read from the output buffer
  uint32_t temp;

  if (sys_inb_cnt(OUT_BUF, &temp) != OK) {
    printf("keyboard_read_buffer::Error using sys_inb_cnt\n");
    return 1;
  }

  //data is a return argument which contains the data read from the buffer
  *data = (uint8_t) temp;
  return 0;
}

bool(keyboard_check_status)() {
  uint8_t status;

  //first reads the status register
  if (keyboard_read_status(&status) != OK) {
    printf("keyboard_check status::Error reading status\n");
    return 1;
  }

  //checks if the ouput buffer has data to read
  if (status & KBC_OUTB) {
    //checks if the data is valid (i.e. if there are no errors and if it comes from the kbd)
    if ((status & (KBC_PARE | KBC_TIMEO | KBC_AUXB)) == 0) {
      return true;
    }
    else {
      return false; //data has errors
    }
  }
  else
    return false; //no data to read
}

int(keyboard_write_command)(uint32_t cmd, uint32_t args) {
  uint8_t status;
  int tries = 0; //counts the number of times the function tries to write to the kbc

  //after trying 3 times to write the command, it gives up
  while (tries < 3) {
    if (keyboard_read_status(&status) != OK) {
      printf("keyboard_write_command::Error from keyboard_read_status\n");
      return 1;
    }

    //checks if it is possible to write to the input buffer
    if ((status & KBC_INPB) == 0) {
      //writes the command
      if (sys_outb(CMD_REG, cmd) != OK) {
        printf("keyboard_write_command::Error writing command to out_buf\n");
        return 1;
      }
      //when the command issued is Write Command Byte it sends the argument to the register
      else if (cmd == WRITE_CMD_B) {
        if (sys_outb(CMD_ARGS_REG, args) != OK) {
          printf("keyboard_write_command::Error writing arguments to register\n");
          return 1;
        }
      }

      return 0;
    }

    //if it didn't succeed, it will count one more try and wait 20ms to give room for the kbc to respond
    tries++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  printf("keyboard_write_command::Exceeded number of tries (3) and KBC was not ready for writing\n");
  return 1;
}

//Deals with the kbd interrupts: it checks if the data is OK and reads it from the output buffer
//The scancode read is passed to lab3.c using the global variable scancode
void(kbc_ih)() {
  if (keyboard_check_status())
    keyboard_read_buffer(&scancode);
}

int kbd_event(event_keyboard *p) {
  static bool two_bytes = false;
  kbc_ih();

  if (scancode == FIRST_OF_TWO_BYTES) {
    two_bytes = true;
    return 1;
  }

  if (two_bytes) {
    switch (scancode) {
      case UP_MAKE:
        *p = up_pressed;
        break;
      case DOWN_MAKE:
        *p = down_pressed;
        break;
      case LEFT_MAKE:
        *p = left_pressed;
        break;
      case RIGHT_MAKE:
        *p = right_pressed;
        break;
      default:
        *p = any_pressed;
        break;
    }

    two_bytes = false;
    return 0;
  }

  switch (scancode) {
    case W_MAKE:
      *p = W_pressed;
      break;
    case A_MAKE:
      *p = A_pressed;
      break;
    case D_MAKE:
      *p = D_pressed;
      break;
    case S_MAKE:
      *p = S_pressed;
      break;
    case SPACE_MAKE:
      *p = space_pressed;
      break;
    case ENTER_MAKE:
      *p = enter_pressed;
      break;
    case ESC_KEY:
      *p = ESC_pressed;
      break;
    default:
      *p = any_pressed;
      break;
  }

  return 0;
}
