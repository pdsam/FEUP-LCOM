#pragma once
#include <lcom/lcf.h>

#include "sprite.h"
#include "stdlib.h"
//#include "pixmap.h"


typedef struct {
  int x,y;             /**< current sprite position */
  int width, height;   /**< sprite dimensions */
  int vx, vy;  /**< current speeds in the x and y direction */
  char *map;           /**< the sprite pixmap (use read_xpm()) */
} Sprite;


Sprite * create_sprite(const char *xpm[], uint16_t x, uint16_t y, int vx, int vy);

void draw_sprite(Sprite *sp);

void destroy_sprite(Sprite *sp);

int animate_sprite(Sprite *sp, int xf, int yf, int nFrames_between_updates);
