#ifndef _PLAYER_H
#define _PLAYER_H
#include <lcom/lcf.h>
#include "v_gr.h"

/** @defgroup player player
 * @{
 *
 * Player class the and fuctions to interact with it
 */


static unsigned int default_speed = 3;  ///< default speed for the players (in pixels/frame)
static unsigned int default_boosts = 3; ///< default number of boosts for the players
static uint8_t boost_duration = 61; ///< default duration of boosts (in frames)
static uint16_t trail_size = 3; ///< trail's height (in pixels); the width is defined by the players speed. Used for draw_trail

typedef struct player {
  char facing;              ///< direction to where the player is moving
  unsigned int trail_color; ///< the motorcicle trail color
  unsigned int speed;       ///< speed of the player, can be changed by boost
  unsigned int boosts;      ///< number of boosts left for the player
  uint8_t boost_counter;    ///< number of ticks the boost has been active (check inc_boost())
  int x, y;                 ///< player position
} player_t;

/**
 *  @brief Creates a new player, acts as constructor for Class player_t
 * 
 *  @param trail_color the color chosen by the player to its trail
 *  @param left boolean value that is true if the player starts the game on the left side and false otherwise
 *  
 *  @return NULL pointer if the memory allocation failed, C pointer to the created player if everything went OK
**/
player_t *new_player(uint32_t trail_color, bool left);

/**
 *  @brief Destructor for player_t. Frees the allocated memory for the player
 * 
 *  @param player C pointer to the player to be deleted
**/
void delete_player(player_t *player);

/**
 *  @brief Sets the direction in which the player is moving
 * 
 *  @param player C pointer to the player
 *  @param direction Char with the new direction of the player. Must be N,S,W or E
 * 
 *  @return -1 if the direction isn't valid, 0 if the direction was set to player
**/
int set_facing(player_t *player, char direction);

/**
 *  @brief Makes the player/motorcicle move in the direction it is facing
 * 
 *  @param player C pointer to the player
**/
void move(player_t *player);

/**
 *  @brief Sets the boost speed to a player.
 *  If force isn't set, cheks if the player isn't being boosted
 *  and if it still has boosts available. If force is set, the player
 *  is boosted no matter what.
 * 
 *  @param player C pointer to the player to be boosted
 *  @param force Boolean value to force the boost activation
**/
void boost(player_t *player, bool force);

/**
 *  @brief Checks if the player is currently boosted
 * 
 *  @param player C pointer to the player to be boosted
 * 
 *  @return true if the player is boosted, false otherwise
**/
bool boosted(player_t *player);

/**
 *  @brief Increments the boost counter by one to control the time the boost has been active
 * 
 *  @param player C pointer to the player
**/
void inc_boost(player_t *player);

/**
 *  @brief Draws the trail of a player. Calls check_collision before drawing
 * 
 *  @param player C pointer to the player being drawn
 * 
 *  @return 0 -> there was no collision
 *  @return 1 -> there was collision
 */
int draw_trail(player_t *player);

/**
 *  @brief Checks for collision
 * 
 *  @param x X position of the player
 *  @param y Y position of the player
 * 
 *  @return 0 -> there was no collision
 *  @return 1 -> there was collision
 */
int check_collision(int x, int y);

/** @} end of player */

#endif
