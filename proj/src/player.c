#include "player.h"

player_t *new_player(uint32_t trail_color, bool left) {
  player_t *pl = malloc(sizeof(player_t));

  if (pl == NULL)
    return NULL; //the memory allocation wasn't successful

  pl->trail_color = trail_color;
  pl->speed = default_speed;
  pl->boosts = default_boosts;
  pl->boost_counter = 0;

  //if the player starts on the left it will be facing east
  //otherwise it will be facing west
  if (left) {
    pl->facing = 'E';
    pl->x = 200; //starting on the left quarter of the screen
  }
  else {
    pl->facing = 'W';
    pl->x = 600; //starting on the right quarter of the screen
  }

  pl->y = 300; //starts in the middle of the vertical resolution

  //TODO add the xpm's
  return pl;
}

void delete_player(player_t *player) {
  free(player); //deletes the player struct
}

int set_facing(player_t *player, char direction) {
  if (direction != 'N' && direction != 'S' && direction != 'W' && direction != 'E')
    return -1;
  else {
    switch (player->facing) {
      case 'N':
        if (direction == 'S')
          return -1;
        break;
      case 'S':
        if (direction == 'N')
          return -1;
        break;
      case 'E':
        if (direction == 'W')
          return -1;
        break;
      case 'W':
        if (direction == 'E')
          return -1;
        break;
    }
    player->facing = direction;
    return 0;
  }
}

void move(player_t *player) {
  switch (player->facing) {
    case 'N':
      player->y -= player->speed;
      break;

    case 'S':
      player->y += player->speed;
      break;

    case 'E':
      player->x += player->speed;
      break;

    case 'W':
      player->x -= player->speed;
      break;
  }

  return;
}

void boost(player_t *player, bool force) {
  if(force){
    if(player->speed == default_speed)
      player->speed = 2 * player->speed;
    
    player->boosts = 0;
  }
  else if (player->boosts != 0 && player->boost_counter == 0) {
    player->speed = 2 * player->speed;
    player->boosts--;
    player->boost_counter = 1;
  }
}

bool boosted(player_t *player) {
  return player->boost_counter != 0;
}

void inc_boost(player_t *player) {
  player->boost_counter++;
  if (player->boost_counter == boost_duration) {
    player->boost_counter = 0;
    player->speed = default_speed;
  }
}

int draw_trail(player_t *player) {
  uint16_t prop = player->speed / trail_size; //when the speed is greater than the trail_size (player being boosted) we use the proportion between both to draw as many squares as needed
  int x = player->x, y = player->y;

  if (player->speed / trail_size == 1) {
    if (check_collision(x, y) != 0) {
      return 1;
    }
    vg_draw_rectangle(x, y, trail_size, trail_size, player->trail_color);
  }
  else {
    switch (player->facing) {
      case 'N':
        for (int i = prop - 1; i >= 0; i--) {
          if (check_collision(x, y + (i * trail_size)) != 0) {
            return 1;
          }
          vg_draw_rectangle(x, y + (i * trail_size), trail_size, trail_size, player->trail_color);
        }
        break;

      case 'S':
        for (int i = prop - 1; i >= 0; i--) {
          if (check_collision(x, y - (i * trail_size)) != 0) {
            return 1;
          }
          vg_draw_rectangle(x, y - (i * trail_size), trail_size, trail_size, player->trail_color);
        }
        break;

      case 'E':
        for (int i = prop - 1; i >= 0; i--) {
          if (check_collision(x - (i * trail_size), y) != 0) {
            return 1;
          }
          vg_draw_rectangle(x - (i * trail_size), y, trail_size, trail_size, player->trail_color);
        }
        break;

      case 'W':
        for (int i = prop - 1; i >= 0; i--) {
          if (check_collision(x + (i * trail_size), y) != 0) {
            return 1;
          }
          vg_draw_rectangle(x + (i * trail_size), y, trail_size, trail_size, player->trail_color);
        }
        break;
    }
  }

  return 0;
}

int check_collision(int x, int y) {
  if (x >= get_h_res() - trail_size || x < 0)
    return 1;
  if (y >= get_v_res() - trail_size || y < 0) //To change y<0 because of the grid size
    return 1;

  for (uint8_t i = 0; i < trail_size; i++) {
    for (uint8_t j = 0; j < trail_size; j++) {
      if (get_pixel(x + i, y + j) != 0) {
        return 1;
      }
    }
  }

  return 0;
}
