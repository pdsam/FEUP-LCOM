#include <lcom/lcf.h>
#include "sprite.h"
#include "v_gr.h"




Sprite* create_sprite(const char *xpm[], uint16_t x, uint16_t y, int vx, int vy){
  Sprite *sp = (Sprite *) malloc (sizeof(Sprite));
  if(sp == NULL){
    printf("error creating sprite");
    return NULL;
  }

  sp->map = read_xpm(xpm,&(sp->width),&(sp->height));
  if(sp->map == NULL){
    free(sp);
    return NULL;
  }
  sp->x = x;
  sp->y = y;

  sp->vx = vx;
  sp->vy = vy;

return sp;
}

void draw_sprite(Sprite *sp){
  uint32_t color;
  int i =0,j=0;
  while(i<sp->height){
    i++;
    j=0;
    while (j<sp->width) {
      j++;
      color = sp->map[i * sp->width + j];
      vbe_draw_pixel(sp->x+j,sp->y+i,color);
    }
  }

}

void destroy_sprite(Sprite *sp){
  int i =0,j=0;
  while(i<sp->height){
    i++;
    j=0;
    while (j<sp->width) {
      j++;
      vbe_draw_pixel(sp->x+j,sp->y+i,0);
    }
  }

}



int animate_sprite(Sprite *sp, int xf, int yf, int nFrames_between_updates){
  static int nframes =0;

  nframes++;

  if(nframes < nFrames_between_updates){
    return 1;
  }

  nframes = 0;

  destroy_sprite(sp);

  sp->x += sp->vx;
  sp->y += sp->vy;

  if(sp->vx > 0) {
  if (sp->x >= xf) {
    sp->x = xf;
    sp->y = yf;

    draw_sprite(sp);
    return 0;
  }
} else if(sp->vy > 0){
  if (sp->y >= yf) {
    sp->x = xf;
    sp->y = yf;

    draw_sprite(sp);
    return 0;
  }
} else if (sp->vx < 0) {
  if (sp->x <= xf) {
    sp->x = xf;
    sp->y = yf;

    draw_sprite(sp);
    return 0;
  }
} else if(sp->vy < 0) {
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
