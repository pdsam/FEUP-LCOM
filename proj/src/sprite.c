#include "sprite.h"
#include <lcom/lcf.h>

Sprite *create_sprite(const char *xpm[], uint16_t x, uint16_t y, int vx, int vy) {
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
  if (sp == NULL) {
    printf("error creating sprite");
    return NULL;
  }

  sp->map = (uint8_t*) read_xpm(xpm, &(sp->width), &(sp->height));
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }
  sp->x = x;
  sp->y = y;

  sp->vx = vx;
  sp->vy = vy;

  return sp;
}

Sprite *create_sprite_direct(xpm_string_t *name, uint16_t x, uint16_t y, int vx, int vy) {
  Sprite *sp = malloc(sizeof(Sprite));

  if (sp == NULL) {
    printf("error creating sprite");
    return NULL;
  }
  enum xpm_image_type temp = XPM_8_8_8;
  xpm_image_t holder;
  sp->map = xpm_load(name, temp, &holder);
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }
  sp->x = x;
  sp->y = y;

  sp->vx = vx;
  sp->vy = vy;
  sp->height = holder.height;
  sp->width = holder.width;

  return sp;
}

void draw_sprite(Sprite *sp) {
  uint32_t color;
  int i = 0, j = 0;
  unsigned int counter = 0;
  while (i < sp->height) {
    i++;
    j = 0;
    while (j < sp->width) {
      j++;
      color = 0;
      color = color | sp->map[counter] 
                    | (sp->map[counter + 1] << 8)
                    | (sp->map[counter + 2] << 16);

      if(sp->x + j < 800 & sp->y + i < 600){
        vbe_draw_pixel(sp->x + j, sp->y + i, color);
      }
      
      counter += 3;
    }
  }
}


void destroy_sprite(Sprite *sp) {
  int i = 0, j = 0;
  while (i < sp->height) {
    i++;
    j = 0;
    while (j < sp->width) {
      j++;
      vbe_draw_pixel(sp->x + j, sp->y + i, 0x000000);
    }
  }
}

int animate_sprite(Sprite *sp, int xf, int yf, int nFrames_between_updates) {
  static int nframes = 0;

  nframes++;

  if (nframes < nFrames_between_updates) {
    return 1;
  }

  nframes = 0;

  destroy_sprite(sp);

  sp->x += sp->vx;
  sp->y += sp->vy;

  if (sp->vx > 0) {
    if (sp->x >= xf) {
      sp->x = xf;
      sp->y = yf;

      draw_sprite(sp);
      return 0;
    }
  }
  else if (sp->vy > 0) {
    if (sp->y >= yf) {
      sp->x = xf;
      sp->y = yf;

      draw_sprite(sp);
      return 0;
    }
  }
  else if (sp->vx < 0) {
    if (sp->x <= xf) {
      sp->x = xf;
      sp->y = yf;

      draw_sprite(sp);
      return 0;
    }
  }
  else if (sp->vy < 0) {
    if (sp->y <= yf) {
      sp->x = xf;
      sp->y = yf;

      draw_sprite(sp);
      return 0;
    }
  }

  draw_sprite(sp);
  return 1;
}

void load_background(Sprite *sp){
  draw_sprite(sp);
  memcpy(get_background_pointer(),get_buffer_pointer(),800*600*3);
}
