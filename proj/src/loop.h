#ifndef _LOOP_H
#define _LOOP_H

/** @defgroup loop loop
 * @{
 *
 * Program flow related functions
 */

/**
 * @brief Changes to video mode, Subscribes all the peripherals and sets them up with the used configuration,inicializes all the objects used in logic
 * 
 */
void game_start();

/**
 * @brief Game loop, where it has the driver_recieve()
 * 
 */
void game_loop();

/**
 * @brief frees the memory used in game_Start() and unsubcribes the peripherals.
 * 
 */
void game_end();

/** @} end of loop */

#endif
