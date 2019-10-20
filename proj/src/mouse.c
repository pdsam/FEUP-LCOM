#include <lcom/lcf.h>
#include <stdint.h>

#include "i8042.h"
#include "mouse.h"
#include <math.h>

int mouse_hookID = 2; //hook_id used for the mouse
uint8_t data;

int(mouse_subscribe_int)(uint8_t *irq_bitmask) {
  //irq_bitmask is a return argument which contains the bit mask created from mouse_hook_id
  *irq_bitmask = BIT(mouse_hookID);

  //subscribes notifications from keyboard based on hook_id passed as 3rd argument
  //from now on the IRQ line 12 will be masked
  if (sys_irqsetpolicy(MOUSE_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &mouse_hookID) != OK) {
    printf("sys_irqsetpolicy::Error setting policy\n");
    return 1;
  }

  //Everything went as expected
  return 0;
}

int(mouse_unsubscribe_int)() {
  //Removes the subscription from the mask created with mouse_hookID
  //IRQ line 12 will be unmasked
  if (sys_irqrmpolicy(&mouse_hookID) != OK) {
    printf("mouse_unsubscribe_int::Error unsubscribing the previous subscription of the interrupt notification on the IRQ line!\n");
    return 1;
  }
  //Everything went as expected
  return 0;
}

int(kbc_read_status)(uint8_t *status) {
  //temporary variable to save what's read from the status register
  uint32_t temp;

  if (sys_inb(STATUS_REG, &temp) != OK) {
    printf("kbc_read_status::Error using sys_inb\n");
    return 1;
  }

  //status is a return argument which contains the status byte
  *status = (uint8_t) temp;
  return 0;
}

int(kbc_read_buffer)(uint8_t *data) {
  //temporary variable to save what's read from the output buffer
  uint32_t temp;

  if (sys_inb(OUT_BUF, &temp) != OK) {
    printf("kbc_read_buffer::Error using sys_inb\n");
    return 1;
  }

  //data is a return argument which contains the data read from the buffer
  *data = (uint8_t) temp;
  return 0;
}

bool(kbc_check_out_buf)() {
  uint8_t status;//temporary variable for status

  //first reads the status register
  if (kbc_read_status(&status) != OK) {
    printf("kbc_check_out_buf::Error reading status\n");
    return 1;
  }

  //checks if the ouput buffer has data to read
  if (status & KBC_OUTB) {
    //checks if the data is valid (i.e. if there are no errors)
    if ((status & (KBC_PARE | KBC_TIMEO)) == 0) {
      return true;
    }
    else {
      return false; //data has errors
    }
  }
  else
    return false; //no data to read
}

bool(kbc_check_in_buf)() {
  uint8_t status;//temporary variable for status

  //first reads the status register
  if (kbc_read_status(&status) != OK) {
    printf("kbc_check_in_buf::Error reading status\n");
    return 1;
  }

  //checks if the input buffer is full
  if ((status & KBC_INPB) == 0) {
    return true;
  }
  else
    return false; //no data to read
}

int(kbc_write_command)(uint32_t cmd) {
  //uint32_t tmp;
  int tries = 0; //counts the number of times the function tries to write to the kbc

  while (tries < 3) {
    if (kbc_check_in_buf()) {
      //writes the command
      if (sys_outb(CMD_REG, cmd) != OK) {
        printf("kbc_write_command::Error writing command to out_buf\n");
        return 1;
      }
      return 0;
    }

    //if it didn't succeed, it will count one more try and wait 20ms to give room for the kbc to respond
    tries++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  printf("bc_write_command::Exceeded number of tries (3) and KBC was not ready for writing\n");
  return 1;
}

int(kbc_write_argument)(uint32_t arg) {
  int tries = 0; //counts the number of times the function tries to write to the kbc

  while (tries < 3) {
    if (kbc_check_in_buf()) {
      //writes the argument
      if (sys_outb(CMD_ARGS_REG, arg) != OK) {
        printf("kbc_write_argument::Error writing command to out_buf\n");
        return 1;
      }

      return 0;
    }

    //if it didn't succeed, it will count one more try and wait 20ms to give room for the kbc to respond
    tries++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  printf("kbc_write_argument::Exceeded number of tries (3) and KBC was not ready for writing\n");
  return 1;
}

int(mouse_write_command)(uint32_t cmd, uint8_t *resp) {
  uint32_t tmp;
  int tries = 0; //counts the number of times the function tries to write to the kbc

  while (tries < 3) {
    if (kbc_check_in_buf()) {
      //writes the argument
      if (sys_outb(CMD_ARGS_REG, cmd) != OK) {
        printf("mouse_write_command::Error writing command to out_buf\n");
        return 1;
      }

      tickdelay(micros_to_ticks(DELAY_US));

      if (sys_inb(OUT_BUF, &tmp) != OK) {
        printf("mouse_write_command::Error reading response from out_buf\n");
        return 1;
      }
      *resp = (uint8_t) tmp;
      return 0;
    }

    //if it didn't succeed, it will count one more try and wait 20ms to give room for the kbc to respond
    tries++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  printf("mouse_write_command::Exceeded number of tries (3) and KBC was not ready for writing\n");
  return 1;
}

int(kbc_command_handler)(uint32_t cmd, uint32_t arg) {
  uint8_t resp;
  uint8_t tries = 0;

  do {//chekc the type of command and handles it's Acknowledgement
    if (kbc_write_command(cmd) != OK) {
      printf("kbc_command_handler::Error writing command\n");
      return 1;
    }

    if (cmd == WRITE_B_MOUSE) {
      if (mouse_write_command(arg, &resp) != OK) {
        printf("kbc_command_handler::Error writing mouse command\n");
        return 1;
      }

      if (resp == ACK) {
        return 0;
      }
      else if (resp == NACK) {
        tries++;
      }
      else if (resp == ERROR) //2 NACKS = ERROR, so the function will always return smth
      {
        printf("kbc_command_handler::Writing the mouse resulted in ERROR\n");
        return 1;
      }
    }
    else if (cmd == WRITE_CMD_B) {
      if (kbc_write_argument(arg) != OK) {
        printf("kbc_command_handler::Error writing argument\n");
        return 1;
      }
      return 0;
    }

  } while (tries < 3);

  printf("kbc_command_handler::Command was not issued, exceeded number of tries(3)\n");
  return 1;
}

void(mouse_parse_packet)(struct packet *pk, uint8_t bytes[], uint8_t size) {
  //checking bit of the first byte for button clicks
  pk->rb = (bytes[0] & PK_RB);
  pk->mb = (bytes[0] & PK_MB);
  pk->lb = (bytes[0] & PK_LB);

//processing the distance and it's sign
  int16_t dx = bytes[1], dy = bytes[2];

  if ((bytes[0] & PK_XSIGN) != 0)
    dx = dx - 256;
  if ((bytes[0] & PK_YSIGN) != 0)
    dy = dy - 256;
//storying the correct values
  pk->delta_x = dx;
  pk->delta_y = dy;
  pk->x_ov = (bytes[0] & PK_XOVF);
  pk->y_ov = (bytes[0] & PK_YOVF);

  for (int i = 0; i < size; i++) {
    pk->bytes[i] = bytes[i];
  }
}
//mouse interrupt handler
void(mouse_ih)() {
  if (kbc_check_out_buf()) {
    kbc_read_buffer(&data);
  }
}

struct mouse_ev* mouse_get_event(struct packet *pk) {
  static struct packet oldpk; // storing the previous packet for comparison
  static struct mouse_ev event;
  static bool first = true;

  uint8_t temp = pk->bytes[0];

  if (first) {
    event.type = BUTTON_EV;
    first = false;
  }
  else {
    if ((temp & PK_LB) == PK_LB && ((oldpk.bytes[0] & PK_LB) == 0)) {
      event.type = LB_PRESSED;
    }
    else if ((temp & PK_RB) == PK_RB && ((oldpk.bytes[0] & PK_RB) == 0)) {
      event.type = RB_PRESSED;
    }
    else if ((temp & PK_MB) == PK_MB && ((oldpk.bytes[0] & PK_MB) == 0)) {
      event.type = BUTTON_EV;
    }
    else if (((temp & PK_LB) == 0) && ((oldpk.bytes[0] & PK_LB) == PK_LB)) {
      event.type = LB_RELEASED;
    }
    else if (((temp & PK_RB) == 0) && ((oldpk.bytes[0] & PK_RB) == PK_RB)) {
      event.type = RB_RELEASED;
    }
    else if (((temp & PK_MB) == 0) && ((oldpk.bytes[0] & PK_MB) == PK_MB)) {
      event.type = BUTTON_EV;
    }
    else if (pk->bytes[1] != 0 || pk->bytes[2] != 0) {
      event.type = MOUSE_MOV;
      event.delta_x = pk->delta_x;
      event.delta_y = pk->delta_y;
    }
  }

  oldpk = *pk;
  return &event;
}
