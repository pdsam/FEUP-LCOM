#pragma once
#include "player.h"
#include "sprite.h"

/** @defgroup game game
 * @{
 *
 * Game Struct and functions to interact with it
 */


/**
 * @brief Struct game_t. Acts as class GAME.
 * Holds game specific information (players, game time and players' points).
 */
typedef struct game_class {
  player_t *p1;       ///< Left player C pointer
  player_t *p2;       ///< Right player C pointer
  uint16_t game_time; ///< Current time left in game
  uint8_t p1_points;  ///< Number of rounds won by player 1
  uint8_t p2_points;  ///< Number of rounds won by player 2
  bool running;       ///< When true the game is running, when false the game is paused

  Sprite *player1_hud; ///< Player 1's corner information
  Sprite *player2_hud; ///< Player 2's corner information
} game_t;

/**
 * @brief Enumeration round_winner_t. Used to tell which player won the round.
 * It can either be player 1, player 2 or a draw. If the round hasn not ended yet
 * then this enumeration can take value no_winner(0)
 */
typedef enum round_winner {
  no_winner, ///< Round not finished
  draw,      ///< Both players collided at the same time -> Draw
  player1,   ///< Player 2 collided -> Player 1 wins
  player2    ///< Player 1 collided -> Player 2 wins
} round_winner_t;

/**
 *  @brief Creates a new game object.
 * 
 *  @retun C pointer to the newly created game.
 */
game_t *new_game();

/**
 *  @brief Deletes the game passed as argument.
 * 
 *  @param game C pointer to the game object
 */
void delete_game(game_t *game);

/**
 *  @brief Sets the direction of a given player in the game.
 *  The directions can be N, S, W or E.
 *  NOTE: the player can't go on the opposite direction it is currently facing.
 *  Otherwise it would crash instantly.
 * 
 *  @param game C pointer to the game object holding the players
 *  @param p1 If true, the direction of the player 1 will be changed.
 *  If false, the direction of the player 2 will be changed.
 */
void set_direction(game_t *game, bool p1, char direction);

/**
 *  @brief Activates the boost of a player if it has any remaining and if the time has not expired
 * 
 *  @param game C pointer to the game object holding the players
 *  @param p1 If true, the player's 1 boost will be activated.
 *  If false, the player's 2 boost will be activated.
 */
void boost_trigger(game_t *game, bool p1);

/**
 * @brief Updates timer and player positon by a frame
 * 
 * @param game C pointer to the game object
 */
int game_tick(game_t *game);

/**
 * @brief Resets the game to its initial state
 * 
 * @param game C pointer to the game object
 */
void reset_game(game_t *game);

/**
 * @brief If the game passed as argument is paused, it is unpaused. 
 * If the game is unpaused then it is paused.
 * 
 * @param game C pointer to the game object
 */
void game_runner(game_t *game);

/**
 * @brief Ends the current round. Also checks if the game has ended and which
 * player has winned.
 * 
 * @param game C pointer to the game object
 * @param winner Enumeration value telling which player won the round
 * 
 * @return 0 -> the game has not ended
 * @return 1 -> the game has ended
 */
int end_round(game_t *game, round_winner_t *winner);

/**
 * @brief Draws the HUD that is static (Player corner information)
 * 
 * @param game C pointer to the game object
 */
void draw_static(game_t *game);

/**
 * @brief Draws the boosts of each player according to the number of boosts left for each other.
 * 
 * @param game C pointer to the game object
 */
void draw_boosts(game_t *game);

/**
 * @brief Draws the number of rounds won by each player.
 * 
 * @param game C pointer to the game object
 */
void draw_points(game_t *game);

/**
 * @brief Draws the timer information in the top middle of the screen.
 * 
 * @param game C pointer to the game object
 */
void draw_time(game_t *game);


/** @} end of game */
