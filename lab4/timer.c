#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <lcom/lab2.h>

#include <stdint.h>

#include "i8254.h"

//definition of timer_interrupt_counter: 
//a global variable to count the number of interrupts from the timer
unsigned int timer_interrupt_counter = 0;
//global hook_id to be used in subscribe_int and unsubscribe_int
int hook_id = 0;

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  //the timer argument is tested: must be between 0 and 2
  if(timer > 2 || timer < 0){
  	printf("timer_get_conf::Invalid input, timer index out of range!\n");
    return 1;
  }
  //checking if it can write ReadBack Command
  if(sys_outb(TIMER_CTRL, TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer)) != OK){
		printf("timer_get_conf::Error writing control word to the timer control register!\n");
		return 1;
  }

  uint32_t temp; //temporary variable to hold status byte
  int TIMER;
  //chooses the port to read from depending on the timer passed as argument
  switch(timer)
  {
    case(0):
      TIMER = TIMER_0;
      break;

    case(1):
      TIMER = TIMER_1;
      break;

    case(2):
      TIMER = TIMER_2;
      break;
  }

  if(sys_inb(TIMER, &temp) != OK){
    printf("timer_get_conf::Error receiving the output from the timer");
    return 1;
  }
  //the value pointed by st is set as the status byte of the timer
  *st =(uint8_t) temp;

  return 0;
}

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  //According to the documentation provided and some research,
  //the minimun working frequency is 18.2Hz, thus making the smallest input possible 19Hz
  //Since we work with a timer frequency of 1193182Hz,
  //this will be the maximum frequency we can input (making the initial value 1)
  if(freq < 19 || freq > TIMER_FREQ)
  {
    printf("timer_set_frequency::Invalid frequency, it must be between 19 and 1193182\n");
    return 1;
  }
  //the previous configuration from timer will be saved to RB_result
  uint8_t RB_result;
  timer_get_conf(0, &RB_result);
  //the first 4 bits from the previous configuration are preserved
  RB_result = RB_result & 0x0f;

  //the counter initial value determines the frequency of the interrupts
  //frequency = TIMER_FREQ/initial value, which is equivalent to next line
  uint16_t  initVal = TIMER_FREQ / freq;
  uint8_t controlWord, MSB, LSB;
  //obtaining the MSByte and LSByte from the initial value
  util_get_MSB(initVal, &MSB);
  util_get_LSB(initVal, &LSB);
  //according to the timer passed as argument, the bits 7 and 6 will be chosen
  switch (timer)
  {
    case 0:
      controlWord = TIMER_SEL0;
      break;

    case 1:
      controlWord = TIMER_SEL1;
      break;

    case 2:
      controlWord = TIMER_SEL2;
      break;
  }
  //setting the control word for the timer selected
  /*note that we'll always write MSB after LSB to the timer because
   we can't predict the size of the initial value*/
  if(sys_outb(TIMER_CTRL, controlWord|TIMER_LSB_MSB|RB_result) != OK)
  {
    printf("timer_set_frequency::Error writing control word\n");
    return 1;
  }
  else if(sys_outb(TIMER_0,LSB) != OK) //LSB loaded before MSB
  {
    printf("timer_set_frequency::Error writing timer's initial value\n");
    return 1;
  }
  else if(sys_outb(TIMER_0,MSB) != OK)
  {
    printf("timer_set_frequency::Error writing timer's initial value\n");
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  //bit_no is a return argument which contains the bit mask created from hook_id
  *bit_no = BIT(hook_id);
  //subscribes notifications from timer0 based on hook_id passed as 3rd argument
  //from now on the IRQ line 0 will be masked
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) != OK){
    printf("sys_irqsetpolicy::Error setting policy\n");
    return 1;
  }

  return 0;
}

int (timer_unsubscribe_int)() {
  //unsubscribes notifications based on hook_id passed as argument
  if(sys_irqrmpolicy(&hook_id)!=OK){
    printf("sys_irqrmpolicy::Error removing policy\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() {
  //the handler exclusively increments the counter global variable
  timer_interrupt_counter++;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  //config will hold the result of this function: the configuration asked
  union timer_status_field_val config;
  //4 possible cases depending on the field passed as argument
  switch(field){
    //user wants the status byte as it was read
    case(all):
      config.byte = st;
      break;
    //user wants to know the initialization mode
    case(initial):
      //selecting bits 5 and 4
      switch(st & TIMER_LSB_MSB){
        case(TIMER_LSB):
          config.in_mode = LSB_only;
          break;

        case(TIMER_MSB):
          config.in_mode = MSB_only;
          break;

        case(TIMER_LSB_MSB):
          config.in_mode = MSB_after_LSB;
          break;

        default:
          config.in_mode = INVAL_val;
          break;
      }
      break;
    //user wants to know the counting mode
    case(mode):
      //selecting bits 3, 2 and 1
      switch(st & TIMER_OPERATING_MODE){
        case(TIMER_INT_TERM_COUNT):
          config.count_mode = 0;
          break;
        case(TIMER_HW_RETRIG_ONES):
          config.count_mode = 1;
          break;
        case(TIMER_RATE_GEN):
          config.count_mode = 2;
          break;
        case(BIT(3) | TIMER_RATE_GEN): //alternative to mode 2 (110)
          config.count_mode = 2; 
          break;
        case(TIMER_SQR_WAVE):
          config.count_mode = 3;
          break;
        case(BIT(3) | TIMER_SQR_WAVE): //alternative to mode 3 (111)
          config.count_mode = 3; 
          break;
        case(TIMER_SW_TRIG_STROBE):
          config.count_mode = 4;
          break;
        case(TIMER_HW_TRIG_STROBE):
          config.count_mode = 5;
          break;
        }
      break;
    //user wants to know the counting base (binary or BCD)
    case(base):
      //bit 0 is tested (1 -> BCD, 0 -> binary)
      if((BIT(0) & st)==1)
        config.bcd=true;
      else 
        config.bcd=false;
      break;

    //just in case the user inputs an invalid field
    default: 
      printf("timer_display_conf::Invalid field inserted\n");
      return 1;
   }
  //check if print_config was successfull
  if(timer_print_config(timer,field,config) != OK){
    return 1;
  }

  return 0;
}
