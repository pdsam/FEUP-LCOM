#pragma once

#include "event.h"
#include "game.h"
#include "menu.h"
#include "highscore.h"
#include <stdlib.h>

/** @defgroup logic logic
 * @{
 *
 * Logic Module, converts events from the Peripheral into actions in the game
 */


/**
 * @brief Game State machine
 * 
 */
typedef enum game_state{
    main_menu = 0,
    playing_local=1,
    playing_remote_host=2,
    playing_remote_guest=3,
    high_score=4,
    exit_procedure = 5
} game_state_t;


/**
 * @brief creates a game object in the logic module
 * 
 */
void initialize_game();
/**
 * @brief creates a menu object in the logic module
 * 
 */
void initialize_menu();
/**
 * @brief creates a highscore object in the logic module
 * 
 */
void initialize_highscores();
/**
 * @brief Creates a date to be loaded to the high scores
 * 
 * @param y - year
 * @param m - Month
 * @param d - Day
 */
void initialize_date(uint8_t y, uint8_t m, uint8_t d);
/**
 * @brief Frees the memory of all the objects created for the logic module
 * 
 */
void delete_all();
/**
 * @brief Update the state machine of the logic 
 * 
 * @param state - State to tupdate to 
 */
void update_game_state(int state);
/**
 * @brief Handles the keyboard event logic
 * 
 * @param p - Keyboard event to be processed 
 * @return int - Returns 0 if sucessful or 1 if not.
 */
int kbd_logic(event_keyboard* p);
/**
 * @brief - Handles the timer event logic
 * 
 * @return int - Returns 0 if sucessful or 1 if not.
 */
int timer_logic();
/**
 * @brief Handles the Mouse event logic
 * 
 * @param ev  - Mouse event
 * @return int - Returns 0 if sucessful or 1 if not.
 */
int mouse_logic(struct mouse_ev* ev);
/**
 * @brief Handles the events originated from the serial port
 * 
 * @param event - Serial Port event
 * @return int - Returns 0 if sucessful or 1 if not.
 */
int serial_logic(event_serial event);

/** @} end of logic */
