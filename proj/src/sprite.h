#pragma once
#include <lcom/lcf.h>
#include "resources/xpm.h"
#include "v_gr.h"
#include "stdlib.h"

/** @defgroup sprite sprite
 * @{
 *
 * Sprite struct and the functions to interact with it
 */


typedef struct {
  int x,y;             /**< current sprite position */
  int width, height;   /**< sprite dimensions */
  int vx, vy;  /**< current speeds in the x and y direction */
  uint8_t *map;        /**< the sprite pixmap */
} Sprite;

/**
 * @brief Create a sprite object
 * 
 * @param xpm - xpm to create the sprite from
 * @param x - it's position acording to the x axis
 * @param y - it's position acording to the y axis
 * @param vx - it's velocity in the x direction
 * @param vy - it's velocitu in the y direction
 * @return Sprite* - Returns a pointer to the struct created
 */
Sprite * create_sprite(const char *xpm[], uint16_t x, uint16_t y, int vx, int vy);
/**
 * @brief Draw the Sprite on the memory
 * 
 * @param sp - Sprite to be drawn
 */
void draw_sprite(Sprite *sp);
/**
 * @brief Erases the Sprite From memory
 * 
 * @param sp - Sprite to be erased
 */
void destroy_sprite(Sprite *sp);
/**
 * @brief Erases and redraws the Sprite in the next position creating the ilusion of movemtn
 * 
 * @param sp - sprite to be moved
 * @param xf - final position acording to the x axis
 * @param yf - final position acording to the y axis
 * @param nFrames_between_updates - number of frames between updates
 * @return int - Returns 0 if sucessful and 1 if not
 */
int animate_sprite(Sprite *sp, int xf, int yf, int nFrames_between_updates);

/**
 * @brief Load a background sprite to an auxiliar buffer
 * 
 * @param sp - BackGround sprite
 */
void load_background(Sprite *sp);
/**
 * @brief Create a sprite from a direct colour mode xpm
 * 
 * @param name file name of the xpm to load 
 * @param x X coordinate of the Sprite's position
 * @param y Y coordinate of the Sprite's position
 * @param vx velocity X axis
 * @param vy velocity Y axis
 * @return Sprite* Pointer to the sprite
 */
Sprite* create_sprite_direct(xpm_string_t *name, uint16_t x, uint16_t y, int vx, int vy);

/** @} end of sprite */
