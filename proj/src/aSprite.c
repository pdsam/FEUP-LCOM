#include "aSprite.h"
#include "resources/xpm.h"
#include <lcom/lcf.h>

AnimSprite *create_asprite(Sprite *base, uint16_t num_xpms, int speed) {
  AnimSprite *asp = malloc(sizeof(AnimSprite));
  asp->max_num_fig = num_xpms;
  asp->aspeed = speed;
  asp->maps[0] = base;
  asp->cur_fig = 0;
  asp->cur_aspeed = speed;
  asp->num_fig = 1;

  return asp;
}

int load_map_to_AnimSprite(xpm_string_t *name, AnimSprite *asp) {
  if (asp->num_fig >= asp->max_num_fig || asp->num_fig >= 10) {
    printf("sprite array full\n");
    return 1;
  }

  int vx, vy;
  uint16_t x, y;

  vx = asp->maps[0]->vx;
  vy = asp->maps[0]->vy;
  x = asp->maps[0]->x;
  y = asp->maps[0]->y;
  asp->maps[asp->num_fig] = create_sprite_direct(name, x, y, vx, vy);
  asp->num_fig++;

  return 0;
}

void draw_AnimSprite(AnimSprite *asp) {
  asp->cur_aspeed--;
  if (asp->cur_aspeed == 0) {
    asp->cur_aspeed = asp->aspeed;
    asp->cur_fig++;
    if (asp->cur_fig == asp->num_fig) {
      asp->cur_fig = 0;
    }
    destroy_sprite(asp->maps[asp->cur_fig]);
    draw_sprite(asp->maps[asp->cur_fig]);
  }
}

void draw_current_Sprite(AnimSprite *asp) {
  destroy_sprite(asp->maps[asp->cur_fig]);
  draw_sprite(asp->maps[asp->cur_fig]);
}

void delete_AnimSprite(AnimSprite *asp) {
  for (int i = 0; i < asp->num_fig; i++) {
    free(asp->maps[i]);
  }
  free(asp);
}
