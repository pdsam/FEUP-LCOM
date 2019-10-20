#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_
#include <lcom/lcf.h>
#include "i8042.h"

extern bool gesture;
//State Machine stuff
enum state {init, drag1, wait, drag2};



//evaluates packets and returns what event happend
struct mouse_ev* mouse_get_event(struct packet *pk);


//actual state machines that checks if the event detected is what the state machine expected
/*State machine states:
    -init: expects a left button click to continue as expected
    -drag1:expects the first leg of the "^" and if the left button is released
    -wait:expects the right button to be holded
    -drag2:expects the second leg of the "^" and if the right button is released
    -end:the mouse moved as expected
  In any state, when it gets an unexpected event, the state machine goes back to "init"

*/
void mouse_detect_gesture(struct mouse_ev *evt, uint8_t x_len, uint8_t tolerance);

#endif
