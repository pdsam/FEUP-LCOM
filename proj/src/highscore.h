#pragma once

#include "menu.h"
#include <lcom/lcf.h>

/** @defgroup highscore HighScore
 * @{
 *
 * Highscore class
 */


#define MAX_SCORES 3 ///< maximum number of scores the highscores menu can show

/**
 *  @brief Enumeration hover_digit_t. Used to know which of the characters of a name
 *  is being hovered by the user.
 */
typedef enum hover_digit {
  no_hover,
  dig1,
  dig2,
  dig3
} hover_digit_t;

/**
 *  @brief Struct score_info_t. Holds the information of a score.
 *  Works as a class SCORE_INFO.
 */
typedef struct score_info {
  char name[3];         ///< name of the player
  uint16_t time_played; ///< duration of the game
  int year;             ///< year in which the score was registed
  int month;            ///< month in which the score was registed
  int day;              ///< day in which the score was registed
  bool done;            ///< boolean value to tell if the player has finished choosing his/her name
} score_info_t;

/**
 *  @brief Struct highscores_t. Holds the information of an highscore menu.
 *  Works as a class HIGHSCORES.
 */
typedef struct highscores {
  score_info_t *scores[MAX_SCORES]; ///< array of all the scores saved (size is defined by MAX_SCORES)

  button_t *back_button; ///< BACK button pointer. This button will be our way back to the main menu
  Sprite *title;         ///< Highscores menu title
  Sprite *rank_col;      ///< Highscores menu first column title - RANK
  Sprite *name_col;      ///< Highscores menu second column title - NAME
  Sprite *time_col;      ///< Highscores menu third column title - TIME
  Sprite *date_col;      ///< Highscores menu fourth column title - DATE
  Sprite *cursor;        ///< Highscores menu cursor sprite
} highscores_t;

/**
 *  @brief Creates a new score based on time played and the current date.
 *  The name starts always as "AAA" and can be edited when inserted in the highscores table.
 * 
 *  @param time Duration of the game
 *  @param year Current year
 *  @param month Current month
 *  @param day Current day
 * 
 *  @return C pointer to the new score created
 */
score_info_t *create_score(uint16_t time, int year, int month, int day);

/**
 *  @brief Deletes the score passed as argument.
 * 
 *  @param score C pointer to the score to be deleted.
 */
void delete_score(score_info_t *score);

/**
 *  @brief Checks if the mouse cursor is hovering any character of the name of the score being
 *  edited at the moment.
 * 
 *  @param cursor C pointer to the cursor Sprite
 * 
 *  @return hover_digit_t value holding which one of the characters (if any) is being hovered
 */
hover_digit_t digit_hover(Sprite *cursor);

/**
 *  @brief Draws a score on the highscores table. Auxiliary function for draw_hs().
 * 
 *  @param scr C pointer to the score
 *  @param y Y coordinate to where the characters should be drawed
 */
void draw_score(score_info_t *scr, uint16_t y);

/**
 *  @brief Creates a new high score menu. Every score is initialized as a NULL pointer.
 * 
 *  @return C pointer to the new highscore menu created.
 */
highscores_t *create_highscores();

/**
 *  @brief Deletes the highscores menu passed as argument.
 * 
 *  @param hs C pointer to the highscores menu to be deleted.
 */
void delete_highscores(highscores_t *hs);

/**
 *  @brief Checks if it is possible to do any operation on the highscores menu.
 *  Useful for optimization purposes, since any editing operation will only be done
 *  if this function returns true.
 * 
 *  @return 0 -> it is not possible to edit anything
 *  @return 1 -> you can edit name, finish, or remove the newly added highscore
 */
int valid_hs_op();

/**
 *  @brief Checks if a click in a position (x,y) was on back button.
 * 
 *  @param hs C pointer to the highscores menu.
 * 
 *  @return 0 -> there was no click.
 *  @return 1 -> there was a click.
 */
int hs_assert_click(highscores_t *hs);

/**
 *  @brief Moves the cursor in the highscores menu
 * 
 *  @param dx X axis displacement
 *  @param dy Y axis displacement
 */
void hs_move_cursor(highscores_t *hs, int16_t dx, int16_t dy);

/**
 *  @brief Sets the newly added highscore as finished and can no longer be edited.
 * 
 *  @param hs C pointer to the highscores menu
 */
void finish_score(highscores_t *hs);

/**
 *  @brief Changes the name in the newly added highscore, character by character
 *  and either to the next letter in the alphabet or the previous
 * 
 *  @param hs C pointer to the highscores menu
 *  @param digit Value holding which one of the characters is being edited
 *  @param up If true, the character is changed to the next in alphabet.
 *  If false the character is changed to the previous letter in the alphabet.
 */
void edit_name(highscores_t *hs, hover_digit_t digit, bool up);

/**
 *  @brief Draws the highscores menu in the video memory
 * 
 *  @param hs C pointer to the highscores menu
 */
void draw_hs(highscores_t *hs);

/**
 *  @brief Checks if a given time is an high score
 *  
 *  @param hs C pointer to the highscores menu
 *  @param time_played Time length of the game
 * 
 *  @return 0 -> the time isn't an high score
 *  @return 1 -> the time is an high score
 */
int is_highscore(highscores_t *hs, uint16_t time_played);

/**
 *  @brief Inserts a new high score into the list
 *  
 *  @param hs C pointer to the highscores menu
 *  @param score C pointer ot the new high score
 */
void insert_highscore(highscores_t *hs, score_info_t *score);

/**
 *  @brief Cancels the insertion of a new high score into the list
 *  
 *  @param hs C pointer to the highscores menu
 */
void cancel_highscore(highscores_t *hs);

/** @} end of highscore */
