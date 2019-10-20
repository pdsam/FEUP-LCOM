// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "v_gr.h"
#include "keyboard.h"
#include "i8042.h"
#include "vMacros.h"
#include "sprite.h"
#include "i8254.h"


extern uint8_t scancode;
extern unsigned int timer_interrupt_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
  return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (video_test_init)(uint16_t mode, uint8_t delay) {


  if(vg_init(mode)== NULL){
    printf("ERROR initializing received mode!\n");
    //return 1;
  }

  sleep(delay);
  vg_exit();

  return 0;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

    uint8_t keyboard_bitmask;

    if (keyboard_subscribe_int(&keyboard_bitmask) != OK) {
      printf("Error subscribing");
      return 1;
    }

    void *v_mem = vg_init(mode);
    if(v_mem == NULL){
      printf("error changing to graphical mode");
      return 1;
    }
    vg_draw_rectangle(x,y,width,height,color);

    bool ESC_flag = false;
    int ipc_status;
    message msg;
    int r;

    while(!ESC_flag) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & keyboard_bitmask) {
            kbc_ih();
            if(scancode == ESC_KEY){
              ESC_flag=true;
            }
          }
          break;
          default:
          break; /* no other notifications expected: do nothing */
        }
      } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
      }
    }

    vg_exit();

    if (keyboard_unsubscribe_int() != OK) {
      printf("Error unsubscribing");
      return 1;
    }

    return 0;
}

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t keyboard_bitmask;
  uint16_t width, height;
  uint16_t x, y;
  uint32_t color = 0;

    if (keyboard_subscribe_int(&keyboard_bitmask) != OK) {
      printf("Error subscribing");
      return 1;
    }

    void *v_mem = vg_init(mode);
    if(v_mem == NULL){
      printf("error changing to graphical mode");
      return 1;
    }

    width = get_h_res()/no_rectangles;
    height = get_v_res()/no_rectangles;

    for(unsigned int i = 0; i < no_rectangles; i++)
    {
      y = i * height;
      for(unsigned int j = 0; j < no_rectangles; j++)
      {
        x = j * width;
        if(mode == 0x105)
          color = col_index(i,j,no_rectangles,step, first);
        else
          color = col_rgb(i,j,step,first);

        vg_draw_rectangle(x,y,width,height,color);
      }
    }

    bool ESC_flag = false;
    int ipc_status;
    message msg;
    int r;

    while(!ESC_flag) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & keyboard_bitmask) {
            kbc_ih();
            if(scancode == ESC_KEY){
              ESC_flag=true;
            }
          }
          break;
          default:
          break; /* no other notifications expected: do nothing */
        }
      } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
      }
    }

    vg_exit();

    if (keyboard_unsubscribe_int() != OK) {
      printf("Error unsubscribing");
      return 1;
    }

    return 0;
}

int (video_test_xpm)(const char *xpm[], uint16_t x, uint16_t y) {

  uint8_t keyboard_bitmask;

  if (keyboard_subscribe_int(&keyboard_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }


    void *v_mem = vg_init(0x115);
    if(v_mem == NULL){
      printf("error changing to graphical mode");
      return 1;
    }

    vg_draw_xpm_direct(xpm,x,y);

    //printf("NOT IN A LOOP \n");

    bool ESC_flag = false;
    int ipc_status;
    message msg;
    int r;

    while(!ESC_flag) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & keyboard_bitmask) {
            kbc_ih();
            if(scancode == ESC_KEY){
              ESC_flag=true;
            }
          }
          break;
          default:
          break; /* no other notifications expected: do nothing */
        }
      } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
      }
    }

    vg_exit();
    if (keyboard_unsubscribe_int() != OK) {
      printf("Error unsubscribing");
      return 1;
    }

    return 0;
  }


  int (video_test_move)(const char *xpm[], uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {

  if (fr_rate > 60){
    fr_rate = 60;
    printf("video_test_move::The maximum refresh rate supported is 60 frames per second. It is now 60 fps \n");
  }
  if (speed == 0) {
    printf("video_test_move::Invalid speed argument! It can't be 0 \n");
  }
  uint8_t keyboard_bitmask, timer_bitmask;
  if (keyboard_subscribe_int(&keyboard_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }
  if (timer_subscribe_int(&timer_bitmask) != OK) {
    printf("Error subscribing");
    return 1;
  }
  void *v_mem = vg_init(VBE_MODE_1024x768_256C);
  if(v_mem == NULL){
    printf("error changing to graphical mode");
    return 1;
  }
  int nFrames_between_updates = -99;
  int n_int_to_frame = TIMER_FREQ / fr_rate;
  int vx,vy;
  //Calculating vx vy
  if (speed > 0) {
    if (abs(xf-xi) > abs(yf-yi)) {
      vx = speed * ((xf-xi) / abs(xf-xi));
      vy = 0;
    } else {
      vy = speed * ((yf-yi) / abs(yf-yi));
      vx = 0;
    }
  } else {
    nFrames_between_updates = abs(speed);
    if (abs(xf-xi) > abs(yf-yi)) {
      vx = 1 * ((xf-xi) / abs(xf-xi));
      vy = 0;
    } else {
      vy = 1 * ((yf-yi) / abs(yf-yi));
      vx = 0;
    }
  }



Sprite* sprite_pointer = create_sprite(xpm, xi,yi,vx,vy);









  bool ESC_flag = false;
  int ipc_status;
  message msg;
  int r;


  while(!ESC_flag) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & keyboard_bitmask) {



          kbc_ih();
          if(scancode == ESC_KEY){
            ESC_flag=true;
            vg_exit();
          }
        }
        if(msg.m_notify.interrupts & timer_bitmask){
          if(timer_interrupt_counter % n_int_to_frame == 0){
            animate_sprite(sprite_pointer, xf , yf , nFrames_between_updates);
          }
          timer_int_handler();
        }

        break;
        default:
        break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }




  if (keyboard_unsubscribe_int() != OK) {
    printf("Error unsubscribing");
    return 1;
  }
    if (timer_unsubscribe_int() != OK) {
      printf("Error unsubscribing");
      return 1;}
      return 1;
    }



      int (video_test_controller)() {
        if(vbe_get_control_parse_info() != 0){
          return -1;
        }

        return 0;
      }
