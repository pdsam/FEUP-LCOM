#include "loop.h"
#include <lcom/lcf.h>
//video
#include "vMacros.h"
#include "v_gr.h"
//keyboard
#include "keyboard.h"
//mouse
#include "mouse.h"
//real time clock
#include "rtc.h"
//#include "timer.h"

#include "event.h"
#include "logic.h"

#include "serialPort.h"

//static bitmasks
static uint8_t keyboard_bitmask;
static uint8_t timer_bitmask;
static uint8_t mouse_bitmask;
static uint8_t rtc_bitmask;
static uint8_t serial_bitmask;

//externs
extern unsigned int timer_interrupt_counter;
extern uint8_t data;

//responsible for video set up and subscriptions
void game_start() {
  uint8_t year, month, day;
  if (!vg_init(VBE_MODE_115)) {
    printf("ERROR initializing received mode!\n");
    return;
  }
  uart_init();
  timer_subscribe_int(&timer_bitmask);
  keyboard_subscribe_int(&keyboard_bitmask);
  mouse_subscribe_int(&mouse_bitmask);
  rtc_subscribe(&rtc_bitmask);
  uart_subscribe(&serial_bitmask);

  rtc_set_periodic_rate(15);

  read_year(&year);
  read_month(&month);
  read_day(&day);
  initialize_date(year, month, day);

  initialize_menu();
  initialize_game();
  initialize_highscores();
  uart_init();
  update_game_state(main_menu);
}

void game_loop() {
  bool loop_stop = false;
  int ipc_status;
  message msg;
  int r;
  //mouse:
  struct packet pk;
  struct mouse_ev *evt;
  uint8_t size = 0; //size of the array of the packets
  uint8_t bytes[3]; //array with the bytes of the packets

  while (!loop_stop) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & keyboard_bitmask) {
            event_keyboard event;
            if (kbd_event(&event) == 0) {
              switch (kbd_logic(&event)) {
                case 0:
                  break;

                case 1:
                  rtc_enable_periodic();
                  break;

                case 2:
                  rtc_disable_periodic();
                  break;
              }
            }
          }

          if (msg.m_notify.interrupts & mouse_bitmask) {
            mouse_ih();
            if (size == 0 && (data & BIT(3)) == 0) { //checks if in sync
              continue;
            }
            else { //checks if the array doesn't contain all the bytes of the packet
              bytes[size] = data;
              size++;
            }

            if (size == 3) { //checks if the array has all the bytes from the packet
              mouse_parse_packet(&pk, bytes, size);
              size = 0;

              evt = mouse_get_event(&pk);
              if (mouse_logic(evt))
                loop_stop = true;

            }
            mouse_get_event(&pk);

          }

          if (msg.m_notify.interrupts & timer_bitmask) {
            if (timer_logic())
              rtc_disable_periodic();
          }

          if (msg.m_notify.interrupts & rtc_bitmask) {
            rtc_ih();
          }

          if (msg.m_notify.interrupts & serial_bitmask) {
            event_serial event;
            uart_event(&event);
            if (event != irrelevent_event) {
              serial_logic(event);
            }
            printf("serial interrupt\n");
          }

          break;

        default:
          break;
      }
    }
    else {
    }
  }
}

void game_end() {
  rtc_set_periodic_rate(6);
  delete_all();
  free(get_buffer_pointer());
  free(get_background_pointer());
  uart_delete_buffers();
  vg_exit();
  uart_unsubscribe();
  rtc_unsubscribe();
  mouse_unsubscribe_int();
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
}
