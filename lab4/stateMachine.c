#include "stateMachine.h"
#include <i8042.h>
#include <lcom/lcf.h>

bool gesture = false;

struct mouse_ev *mouse_get_event(struct packet *pk) {
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

void mouse_detect_gesture(struct mouse_ev *evt, uint8_t x_len, uint8_t tolerance) {
  static enum state st = init;
  static bool accepted_line = false;
  static int16_t sum_x = 0;
//based on the state of the state machine
//it expects and handles the events returned by the mouse_get_event
  switch (st) {

    case init:
      if (evt->type == LB_PRESSED) {
        st = drag1;
      }
      break;

    case drag1:
      if (evt->type == MOUSE_MOV) {
        //na pratica acontace muitas vezes um dos deslocamentos ser nulo.
        //para facilitar, nesse caso incrementamos ambos os deslocamentos para haver um declive
        if(evt->delta_y  == 0 || evt->delta_x == 0)
        {
          evt->delta_y += 1;
          evt->delta_x += 1;
        }
        //verifica se nao rompeu a tolerancia e se ja terminou
        int slope = evt->delta_y / evt->delta_x;

        if (slope <= 1 && (abs(evt->delta_x) > tolerance || abs(evt->delta_y) > tolerance)) {
          st = init;
        }
        else {
          sum_x += evt->delta_x;
          if (abs(sum_x) >= x_len) {
            accepted_line = true;
          }
        }
      }
      else if (evt->type == LB_RELEASED && accepted_line) {
        sum_x = 0;
        accepted_line = false;
        st = wait;
      }
      else {
        st = init;
      }
      break;

    case wait:
      if (evt->type == MOUSE_MOV) {
        //verifica se nao rompeu a tolerancia
        if (abs(evt->delta_x) > tolerance || abs(evt->delta_y) > tolerance) {
          st = init;
        }
      }
      else if (evt->type == RB_PRESSED) {
        st = drag2;
      }
      else if (evt->type == LB_PRESSED) {
        st = drag1;
      }
      else{
        st = init;
      }
      break;

    case drag2:
      if (evt->type == MOUSE_MOV) {
        //verifica se nao rompeu a tolerancia e se ja terminou
        int slope = evt->delta_y / evt->delta_x;
        if (slope >= -1 && (abs(evt->delta_x) > tolerance || abs(evt->delta_y) > tolerance)) {
          st = init;
        }
        else {
          sum_x += evt->delta_x;
          if (abs(sum_x) >= x_len) {
            accepted_line = true;
          }
        }
      }
      else if (evt->type == RB_RELEASED && accepted_line) {
        sum_x = 0;
        accepted_line = false;
        gesture = true;
        return;
      }
      else {
        st = init;
      }
      break;
  }
}
