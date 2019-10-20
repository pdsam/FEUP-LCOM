#pragma once

#include "aSprite.h"
#include "proj_macros.h"
#include "v_gr.h"
#include <lcom/lcf.h>

/** @defgroup menu menu 
 * @{
 *
 * Menu class and the functions to interact with it
 */

/**
 *  @brief Enumeration click_t. Used to established values for different button clicks.
**/
typedef enum click {
  local_click = 0, ///< There was a click on Local Mode button
  remote_click,    ///< There was a click on Remote Mode button
  hs_click,        ///< There was a click on High Scores button
  exit_click,      ///< There was a click on Exit button
  no_click = -1    ///< There was no click on a button
} click_t;

/**
 *  @brief Struct button_t. Acts as class BUTTON.
 *  A button has two xpms: a default and a hovered xpm.
 *  The highlighted atribute tells which one of the xpms should be used.
 *  A button also holds information about the coordinates from where it starts and to where it ends.
**/
typedef struct button {
  uint16_t xi;      ///< Button's left boundary
  uint16_t yi;      ///< Button's top boundary
  uint16_t xf;      ///< Button's right boundary
  uint16_t yf;      ///< Button's bottom boundary
  bool highlighted; ///< True when the button is being hovered, false otherwise

  Sprite *default_map;     ///< Button's default xpm
  Sprite *highlighted_map; ///< Button's xpm when being hovered
} button_t;

/**
 *  @brief Struct menu_t. Acts as class MENU.
 *  A menu has 4 buttons: Local Mode, Remote Mode, High Scores and Exit.
 *  Buttons aside, it also holds the title information, an animated coin spining
 *  and a mouse cursor sprite.
**/
typedef struct menu {
  Sprite *title;           ///< Menu's title
  button_t *local_button;  ///< Local Mode button
  button_t *remote_button; ///< Remote Mode button
  button_t *hs_button;     ///< High Scores button
  button_t *exit_button;   ///< Exit button
  uint8_t n_coins;         ///< Number of coins remaining
  AnimSprite *coin;        ///< Coin animated Sprite
  Sprite *cursor;          ///< Menu's mouse cursor sprite
} menu_t;

/**
 *  @brief Creates a new button
 * 
 *  @return C pointer to the button created
**/
button_t *create_button(uint16_t xi, uint16_t yi, xpm_string_t *default_map, xpm_string_t *highlighted_map);

/**
 *  @brief Deletes an existing button
 * 
 *  @param but C pointer to the button
**/
void delete_button(button_t *but);

/**
 *  @brief Checks if the mouse is hovering a button
 * 
 *  @param but C pointer to button
 *  @param cursor C pointer to the mouse sprite
 * 
 *  @return 0 -> the button isn't being hovered
 *  @return 1 -> the button is being hovered
**/
int mouse_hover(button_t *but, Sprite *cursor);

/**
 *  @brief Creates a new menu
 * 
 *  @return C pointer to the menu created
**/
menu_t *create_menu();

/**
 *  @brief Deletes an existing menu
 * 
 *  @param menu C pointer to the menu
**/
void delete_menu(menu_t *menu);

/**
 *  @brief Checks if a click in a position (x,y) was on a button
 * 
 *  @param menu Menu in which the click is made
 * 
 *  @return click_t enumeration flag that represents the button pressed.
 *      0 -> Local Mode, 1 -> Remote Mode, 2 -> High Scores, -1 -> No button pressed
 */
click_t menu_assert_click(menu_t *menu);

/**
 *  @brief Moves the cursor in the menu
 * 
 *  @param dx X axis displacement
 *  @param dy Y axis displacement
 */
void move_cursor(menu_t *menu, int16_t dx, int16_t dy);

/**
 *  @brief Checks if there still are coins to play
 * 
 *  @param menu C pointer to the menu
 * 
 *  @return 0 -> there are no coins to play
 *  @return 1 -> there is at least one coin to play
 */
int coins_left(menu_t *menu);

/**
 *  @brief Adds one coind to the number of remaining coins
 * 
 *  @param menu C pointer to the menu
 */
void add_coin(menu_t *menu);

/**
 *  @brief Removes one coin from the number of remaining coins
 * 
 *  @param menu C pointer to the menu
 */
void remove_coin(menu_t *menu);

/**
 *  @brief Draws the coin next xpm every 9 ticks (6 times a second)
 * 
 *  @param menu C pointer to the menu
 */
void draw_coin(menu_t *menu);

/**
 *  @brief Draws the number of the remaining coins on the screen
 * 
 *  @param menu C pointer to the menu
 */
void draw_ncoins(menu_t *menu) ;

/**
 *  @brief Draws the menu in the video memory
 * 
 *  @param menu C pointer to the menu
 */
void draw_menu(menu_t *menu);

/** @} end of menu */
