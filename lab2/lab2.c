#include <lcom/lcf.h>

#include <lcom/lab2.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8254.h"

//extern variable from timer.c
//it is declared here (lab2.c) and defined in timer.c for the handler to increment it
extern unsigned int timer_interrupt_counter;

int main(int argc, char *argv[]){ //(int argc, char *argv[])
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  //timer_test_time_base(3,30);
  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  //test if timer input is correct
  if(timer < 0 || timer > 2)
  {
    printf("timer_test_read_config::Invalid timer input\n");
    return 1;
  }

  uint8_t temp;
  //check if timer_get_conf was successful
  if(timer_get_conf(timer, &temp) != OK){
    return 1;}

  if(timer_display_conf(timer,temp, field) != OK){
    return 1;}

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  //check if timer_set_frequency was successfull
  if(timer_set_frequency(timer,freq) != OK)
    return 1;

  return 0;
}

int(timer_test_int)(uint8_t time) {
  //Testing for invalid inputs
  if(time==0)
  {
    printf("timer_test_int::Time interval cannot be zero\n");
    return 1;
  }

  //converting seconds to number of interrupts
  unsigned int interrupt_number_total = time * DEFAULT_FREQ;

  //subscribing timer IRQ (IRQ line is masked acording to irq_bitmask)
  uint8_t irq_bitmask;
  if(timer_subscribe_int(& irq_bitmask) != OK){
   printf("timer_subscribe_int::Subscription did not went through\n");
   return 1;
  }

  //Variables that will be used afterwards for driver_receive()
  int ipc_status;
  message msg;
  int r;


  //Cicle stops when we reach the end of the time interval 
  while( timer_interrupt_counter < interrupt_number_total) {
  // Get a request message
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_bitmask) { // subscribed interrupt

            //handler called to increment the global counter timer_interrupt_counter
            timer_int_handler(); 

            //timer_print_elapsed_time is called every second
            if(timer_interrupt_counter % 60 == 0){ 

              timer_print_elapsed_time();

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

  //unsubscribing timer IRQ
  if(timer_unsubscribe_int() != OK){

  	printf("timer_test_int::Error unsubscribing from the timer interrupt!\n");
  	return 1;

  }
  return 0; //it worked :)
}

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
  //LSB are the 8 least significant bits from val
  val = val & 0xff;
  
  //saves LSB at lsb address passed as argument
  *lsb = (uint8_t) val;

  return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
  //MSB are the 8 most significant bits from val
  val >>= 8;

  //saves MSB at msb address passed as argument
  *msb = (uint8_t) val;

  return 0;
}
