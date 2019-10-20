#include "game.h"
#include "proj_macros.h"
#include <lcom/lcf.h>

static uint16_t game_time = 90;
static uint16_t seconds_counter = 0;
static bool sudden_death = false;

game_t *new_game() {

  game_t *game = malloc(sizeof(game_t));

  if(game == NULL)
    return NULL;

  game->p1 = new_player(0xff9900, 1);
  game->p2 = new_player(0x00ffff, 0);
  game->game_time = game_time;
  game->p1_points = 0;
  game->p2_points = 0;
  game->running = false;
  game->player1_hud = create_sprite_direct(player_one_hud_xpm, 10, 5, 0, 0);
  game->player2_hud = create_sprite_direct(player_two_hud_xpm, 637, 5, 0, 0);
  return game;
}

void delete_game(game_t *game) {
  delete_player(game->p1);
  delete_player(game->p2);
  free(game);
  return;
}

void set_direction(game_t *game, bool p1, char direction) {
  if (!game->running)
    return;
  else if (p1) {
    set_facing(game->p1, direction);
  }
  else {
    set_facing(game->p2, direction);
  }
}

void boost_trigger(game_t *game, bool p1) {
  if (!game->running)
    return;
  else if (p1)
    boost(game->p1, false);
  else
    boost(game->p2, false);

  draw_boosts(game);
}

void game_runner(game_t *game) {
  game->running = !game->running;
}

int game_tick(game_t *game) {
  if (!(game->running)) {
    return 1;
  }

  if (!sudden_death) {
    seconds_counter++;
    if (seconds_counter % TICKS_PER_SECOND == 0) {
      seconds_counter = 0;
      game->game_time--;
      draw_time(game);
    }

    if (game->game_time == 0) {
      sudden_death = true;
      boost(game->p1, true);
      boost(game->p2, true);
      draw_boosts(game);
    }

    if (boosted(game->p1))
      inc_boost(game->p1);

    if (boosted(game->p2))
      inc_boost(game->p2);
  }

  move(game->p1);
  move(game->p2);

  return 0;
}

void reset_game(game_t *game) {
  delete_player(game->p1);
  delete_player(game->p2);
  game->p1 = new_player(0xff9900, 1);
  game->p2 = new_player(0x00ffff, 0);
  game->game_time = game_time;
  sudden_death = false; //when the round ends the timer goes back to normal and sudden death is deactivated
  game->running = false;
}

int end_round(game_t *game, round_winner_t *winner) {
  switch (*winner) {
    case (draw):
      game->p1_points++;
      game->p2_points++;
      break;

    case (player1):
      game->p1_points++;
      break;

    case (player2):
      game->p2_points++;
      break;

    default:
      return 0;
  }

  reset_game(game);
  vg_clear_display();
  draw_static(game);
  draw_boosts(game);
  draw_points(game);
  draw_time(game);

  if (game->p1_points == 3 || game->p2_points == 3) {
    if (game->p1_points == 3 && game->p2_points == 3)
      *winner = draw;
    else if (game->p1_points == 3)
      *winner = player1;
    else if (game->p2_points == 3)
      *winner = player2;

    game->p1_points = 0;
    game->p2_points = 0;
    return 1;
  }
  else {
    return 0;
  }
}

void draw_static(game_t *game) {
  draw_sprite(game->player1_hud);
  draw_sprite(game->player2_hud);
  vg_draw_rectangle(0, 90, get_h_res(), 2, 0xFFFFFF);
}

void draw_boosts(game_t *game) {
  //cleaning previoues numbers
  vg_draw_rectangle(97, 34, 19, 21, 0x00);
  vg_draw_rectangle(724, 34, 19, 21, 0x00);
  //loading the xpm from the counters array
  Sprite *boost1 = create_sprite_direct(counters[game->p1->boosts], 97, 34, 0, 0);
  Sprite *boost2 = create_sprite_direct(counters[game->p2->boosts], 724, 34, 0, 0);

  draw_sprite(boost1);
  draw_sprite(boost2);
  //deleting the temporary sprite to free memory space alocated
  free(boost1);
  free(boost2);
}

void draw_points(game_t *game) {
  Sprite *point1 = create_sprite_direct(counters[game->p1_points], 90, 58, 0, 0);
  Sprite *point2 = create_sprite_direct(counters[game->p2_points], 717, 58, 0, 0);

  draw_sprite(point1);
  draw_sprite(point2);

  free(point1);
  free(point2);
}

void draw_time(game_t *game) {
  //cleaning previoues numbers
  vg_draw_rectangle(346, 30, 33, 38, 0x00);
  vg_draw_rectangle(389, 30, 33, 38, 0x00);
  vg_draw_rectangle(423, 30, 33, 38, 0x00);

  uint16_t mins = game->game_time / 60, segs = game->game_time % 60;
  uint16_t seg1 = segs / 10, seg2 = segs % 10; //seg1 is the left digit of segs, seg2 is the right digit
  //loading the xpms from the numbers array
  Sprite *dots = create_sprite_direct(dots_big_xpm, 380, 45, 0, 0);
  Sprite *min = create_sprite_direct(numbers[mins], 346, 30, 0, 0);
  Sprite *seg_left = create_sprite_direct(numbers[seg1], 389, 30, 0, 0);
  Sprite *seg_right = create_sprite_direct(numbers[seg2], 423, 30, 0, 0);

  draw_sprite(dots);
  draw_sprite(min);
  draw_sprite(seg_left);
  draw_sprite(seg_right);
  //deleting the temporary sprite to free memory space alocated
  free(dots);
  free(min);
  free(seg_left);
  free(seg_right);
}
