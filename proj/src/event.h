#pragma once

/** @defgroup event Event
 * @{
 *
 * All the events for each periferal
 */

/**
 * @brief keyboard related events
 * 
 */
typedef enum keyboard_event {
  W_pressed = 0,
  A_pressed = 1,
  D_pressed = 2,
  S_pressed = 3,
  space_pressed = 4,
  enter_pressed = 5,
  up_pressed = 6,
  down_pressed = 7,
  left_pressed = 8,
  right_pressed = 9,

  W_released = 10,
  A_released = 11,
  D_released = 12,
  S_released = 13,
  space_released = 14,
  enter_released = 15,
  up_released = 16,
  down_released = 17,
  left_released = 18,
  right_released = 19,

  any_pressed = 20,
  any_released = 21,

  ESC_pressed = 25

} event_keyboard;

/**
 * @brief serial port related events
 * 
 */
typedef enum serial_event {
  irrelevent_event = 0,
  up_key = 1,
  down_key = 2,
  right_key = 3,
  left_key = 4,
  boost_key = 5,
  round_start = 6,
  connection_byte = 7,
  player_one_win = 8,
  player_two_win = 9
} event_serial;

/** @} end of event */
