#include "menu.h"
#include <lcom/lcf.h>

button_t *create_button(uint16_t xi, uint16_t yi, xpm_string_t *default_map, xpm_string_t *highlighted_map) {
  button_t *but = malloc(sizeof(button_t));

  if (but == NULL)
    return NULL;
  but->default_map = create_sprite_direct(default_map, xi, yi, 0, 0);
  but->highlighted_map = create_sprite_direct(highlighted_map, xi, yi, 0, 0);
  but->xi = xi;
  but->yi = yi;
  but->xf = xi + but->default_map->width;
  but->yf = yi + but->default_map->height;
  but->highlighted = false;

  return but;
}

void delete_button(button_t *but) {
  free(but->default_map);
  free(but->highlighted_map);
  free(but);
}

int mouse_hover(button_t *but, Sprite *cursor) {
  if (but->xi <= cursor->x && cursor->x <= but->xf && but->yi <= cursor->y && cursor->y <= but->yf)
    return 1;
  else
    return 0;
}

menu_t *create_menu() {
  menu_t *menu = malloc(sizeof(menu_t));
  menu->n_coins = 0;

  menu->cursor = create_sprite_direct(cursor_xpm, 50, 50, 0, 0);
  menu->coin = create_asprite(create_sprite_direct(coin1_xpm, 10, 560, 0, 0), 6, 15);
  load_map_to_AnimSprite(coin2_xpm, menu->coin);
  load_map_to_AnimSprite(coin3_xpm, menu->coin);
  load_map_to_AnimSprite(coin4_xpm, menu->coin);
  load_map_to_AnimSprite(coin5_xpm, menu->coin);
  load_map_to_AnimSprite(coin6_xpm, menu->coin);
  menu->exit_button = create_button(699, 570, exit_xpm, exit_highlight_xpm);
  menu->hs_button = create_button(280, 400, high_scores_xpm, high_scores_highlight_xpm);
  menu->local_button = create_button(280, 300, local_mode_xpm, local_highlight_xpm);
  menu->remote_button = create_button(280, 350, remote_mode_xpm, remote_highlight_xpm);
  menu->title = create_sprite_direct(title_xpm, 61, 75, 0, 0);
  return menu;
}

void delete_menu(menu_t *menu) {
  free(menu->title);
  delete_button(menu->local_button);
  delete_button(menu->remote_button);
  delete_button(menu->hs_button);
  delete_button(menu->exit_button);
  delete_AnimSprite(menu->coin);
  free(menu->cursor);
}

click_t menu_assert_click(menu_t *menu) {
  if (mouse_hover(menu->local_button, menu->cursor))
    return local_click;
  else if (mouse_hover(menu->remote_button, menu->cursor))
    return remote_click;
  else if (mouse_hover(menu->hs_button, menu->cursor))
    return hs_click;
  else if (mouse_hover(menu->exit_button, menu->cursor))
    return exit_click;
  else
    return no_click;
}

void move_cursor(menu_t *menu, int16_t dx, int16_t dy) {
  destroy_sprite(menu->cursor);

  menu->cursor->x += dx;
  if (menu->cursor->x > 790) {
    menu->cursor->x = 790;
  }
  else if (menu->cursor->x < 0) {
    menu->cursor->x = 0;
  }

  menu->cursor->y -= dy;
  if (menu->cursor->y > 590) {
    menu->cursor->y = 590;
  }
  else if (menu->cursor->y < 0) {
    menu->cursor->y = 0;
  }

  if (mouse_hover(menu->local_button, menu->cursor))
    menu->local_button->highlighted = true;
  else if (mouse_hover(menu->remote_button, menu->cursor))
    menu->remote_button->highlighted = true;
  else if (mouse_hover(menu->hs_button, menu->cursor))
    menu->hs_button->highlighted = true;
  else if (mouse_hover(menu->exit_button, menu->cursor))
    menu->exit_button->highlighted = true;
  else {
    menu->local_button->highlighted = false;
    menu->remote_button->highlighted = false;
    menu->hs_button->highlighted = false;
    menu->exit_button->highlighted = false;
  }
}

int coins_left(menu_t *menu) {
  return menu->n_coins > 0;
}

void add_coin(menu_t *menu) {
  if (menu->n_coins < 10)
    menu->n_coins++;
  draw_menu(menu);
}

void remove_coin(menu_t *menu) {
  if (menu->n_coins > 0)
    menu->n_coins--;
  draw_menu(menu);
}

void draw_coin(menu_t *menu) {
  draw_AnimSprite(menu->coin);
}

void draw_ncoins(menu_t *menu) {
  Sprite *times = create_sprite_direct(times_xpm, 40, 567, 0, 0);
  Sprite *number = create_sprite_direct(numbers[menu->n_coins], 60, 557, 0, 0);

  draw_sprite(times);
  draw_sprite(number);

  free(times);
  free(number);
}

void draw_menu(menu_t *menu) {
  draw_background();
  draw_sprite(menu->title);
  if (menu->local_button->highlighted)
    draw_sprite(menu->local_button->highlighted_map);
  else
    draw_sprite(menu->local_button->default_map);

  if (menu->remote_button->highlighted)
    draw_sprite(menu->remote_button->highlighted_map);
  else
    draw_sprite(menu->remote_button->default_map);

  if (menu->hs_button->highlighted)
    draw_sprite(menu->hs_button->highlighted_map);
  else
    draw_sprite(menu->hs_button->default_map);

  if (menu->exit_button->highlighted)
    draw_sprite(menu->exit_button->highlighted_map);
  else
    draw_sprite(menu->exit_button->default_map);

  draw_current_Sprite(menu->coin);
  draw_ncoins(menu);
  draw_sprite(menu->cursor);
}
