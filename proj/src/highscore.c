#include "highscore.h"
#include <lcom/lcf.h>

static int8_t last_index;
static score_info_t *deleted_score;

score_info_t *create_score(uint16_t time, int year, int month, int day) {
  score_info_t *scr = malloc(sizeof(score_info_t));

  if (scr == NULL)
    return NULL;

  for (uint8_t i = 0; i < 3; i++)
    scr->name[i] = 'A';

  scr->time_played = time;
  scr->day = day;
  scr->month = month;
  scr->year = year;

  return scr;
}

void delete_score(score_info_t *score) {
  if (score != NULL)
    free(score);
}

hover_digit_t digit_hover(Sprite *cursor) {
  if (last_index * 50 + 107 <= cursor->y && cursor->y <= last_index * 50 + 143) {
    if (166 <= cursor->x && cursor->x <= 202)
      return dig1;
    if (204 <= cursor->x && cursor->x <= 240)
      return dig2;
    if (242 <= cursor->x && cursor->x <= 278)
      return dig3;
  }

  return no_hover;
}

void draw_name(score_info_t *scr, uint16_t y) {
  uint16_t char1 = ((int) scr->name[0] - 65) % 26;
  uint16_t char2 = ((int) scr->name[1] - 65) % 26;
  uint16_t char3 = ((int) scr->name[2] - 65) % 26;

  Sprite *char1_x = create_sprite_direct(letters[char1], 166, y, 0, 0);
  Sprite *char2_x = create_sprite_direct(letters[char2], 204, y, 0, 0);
  Sprite *char3_x = create_sprite_direct(letters[char3], 242, y, 0, 0);

  draw_sprite(char1_x);
  draw_sprite(char2_x);
  draw_sprite(char3_x);

  free(char1_x);
  free(char2_x);
  free(char3_x);
}

void draw_tm(score_info_t *scr, uint16_t y) {
  uint16_t char1, char2, char3;

  if (scr->time_played >= 999) {
    char1 = char2 = char3 = 9;
  }
  else {
    char1 = scr->time_played / 100;
    char2 = (scr->time_played % 100) / 10;
    char3 = scr->time_played % 10;
  }

  Sprite *char1_x = create_sprite_direct(numbers[char1], 337, y, 0, 0);
  Sprite *char2_x = create_sprite_direct(numbers[char2], 371, y, 0, 0);
  Sprite *char3_x = create_sprite_direct(numbers[char3], 405, y, 0, 0);

  draw_sprite(char1_x);
  draw_sprite(char2_x);
  draw_sprite(char3_x);

  free(char1_x);
  free(char2_x);
  free(char3_x);
}

void draw_day(score_info_t *scr, uint16_t y) {
  uint16_t char1 = scr->day / 10;
  uint16_t char2 = scr->day % 10;

  Sprite *char1_x = create_sprite_direct(numbers[char1], 478, y, 0, 0);
  Sprite *char2_x = create_sprite_direct(numbers[char2], 512, y, 0, 0);
  Sprite *char3_x = create_sprite_direct(stripe_xpm, 546, y, 0, 0);

  draw_sprite(char1_x);
  draw_sprite(char2_x);
  draw_sprite(char3_x);

  free(char1_x);
  free(char2_x);
  free(char3_x);
}

void draw_month(score_info_t *scr, uint16_t y) {
  uint16_t char1 = scr->month / 10;
  uint16_t char2 = scr->month % 10;

  Sprite *char1_x = create_sprite_direct(numbers[char1], 567, y, 0, 0);
  Sprite *char2_x = create_sprite_direct(numbers[char2], 601, y, 0, 0);
  Sprite *char3_x = create_sprite_direct(stripe_xpm, 635, y, 0, 0);

  draw_sprite(char1_x);
  draw_sprite(char2_x);
  draw_sprite(char3_x);

  free(char1_x);
  free(char2_x);
  free(char3_x);
}

void draw_year(score_info_t *scr, uint16_t y) {
  uint16_t char1 = scr->year / 1000;
  uint16_t char2 = (scr->year % 1000) / 100;
  uint16_t char3 = (scr->year % 100) / 10;
  uint16_t char4 = scr->year % 10;

  Sprite *char1_x = create_sprite_direct(numbers[char1], 656, y, 0, 0);
  Sprite *char2_x = create_sprite_direct(numbers[char2], 690, y, 0, 0);
  Sprite *char3_x = create_sprite_direct(numbers[char3], 724, y, 0, 0);
  Sprite *char4_x = create_sprite_direct(numbers[char4], 758, y, 0, 0);

  draw_sprite(char1_x);
  draw_sprite(char2_x);
  draw_sprite(char3_x);
  draw_sprite(char4_x);

  free(char1_x);
  free(char2_x);
  free(char3_x);
  free(char4_x);
}

void draw_score(score_info_t *scr, uint16_t y) {
  draw_name(scr, y);

  draw_tm(scr, y);

  draw_day(scr, y);

  draw_month(scr, y);

  draw_year(scr, y);
}

highscores_t *create_highscores() {
  highscores_t *hs = malloc(sizeof(highscores_t));

  if (hs == NULL) {
    return NULL;
  }

  for (uint8_t i = 0; i < MAX_SCORES; i++) {
    hs->scores[i] = NULL;
  }

  hs->back_button = create_button(0, 0, back_xpm, back_highlight_xpm);
  hs->title = create_sprite_direct(high_scores_title_xpm, 195, 3, 0, 0);
  hs->rank_col = create_sprite_direct(rank_xpm, 10, 57, 0, 0);
  hs->name_col = create_sprite_direct(name_xpm, 165, 57, 0, 0);
  hs->time_col = create_sprite_direct(time_xpm, 332, 57, 0, 0);
  hs->date_col = create_sprite_direct(date_xpm, 568, 57, 0, 0);
  hs->cursor = create_sprite_direct(cursor_xpm, 50, 50, 0, 0);

  last_index = -1;

  return hs;
}

void delete_highscores(highscores_t *hs) {
  for (uint8_t i = 0; i < MAX_SCORES; i++) {
    delete_score(hs->scores[i]);
  }

  delete_button(hs->back_button);
  free(hs->title);
  free(hs->rank_col);
  free(hs->name_col);
  free(hs->time_col);
  free(hs->date_col);
  free(hs->cursor);
}

int valid_hs_op(){
  return last_index != -1;
}

int hs_assert_click(highscores_t *hs) {
  return mouse_hover(hs->back_button, hs->cursor);
}

void hs_move_cursor(highscores_t *hs, int16_t dx, int16_t dy) {
  destroy_sprite(hs->cursor);

  hs->cursor->x += dx;
  if (hs->cursor->x > 790) {
    hs->cursor->x = 790;
  }
  else if (hs->cursor->x < 0) {
    hs->cursor->x = 0;
  }

  hs->cursor->y -= dy;
  if (hs->cursor->y > 590) {
    hs->cursor->y = 590;
  }
  else if (hs->cursor->y < 0) {
    hs->cursor->y = 0;
  }

  if (mouse_hover(hs->back_button, hs->cursor))
    hs->back_button->highlighted = true;
  else
    hs->back_button->highlighted = false;
}

void finish_score(highscores_t *hs) {
  hs->scores[last_index]->done = true;
  last_index = -1;
}

void edit_name(highscores_t *hs, hover_digit_t digit, bool up) {
  int8_t inc;
  score_info_t *scr = hs->scores[last_index];
  if (!scr->done) {
    if (up)
      inc = 1;
    else
      inc = -1;

    switch (digit) {
      case dig1:
        if (scr->name[0] == 'A' && !up)
          scr->name[0] = 'Z';
        else
          scr->name[0] = (char) (((int) scr->name[0]) - 65 + inc) % 26 + 65;
        break;
      case dig2:
        if (scr->name[1] == 'A' && !up)
          scr->name[1] = 'Z';
        else
          scr->name[1] = (char) (((int) scr->name[1]) - 65 + inc) % 26 + 65;
        break;
      case dig3:
        if (scr->name[2] == 'A' && !up)
          scr->name[2] = 'Z';
        else
          scr->name[2] = (char) (((int) scr->name[2]) - 65 + inc) % 26 + 65;
        break;
      default:
        break;
    }
  }
}

void draw_hs(highscores_t *hs) {
  draw_background();

  for (uint8_t i = 0; i < MAX_SCORES; i++) {
    if (hs->scores[i] != NULL) {
      Sprite *temp = create_sprite_direct(numbers[i + 1], 45, i * 50 + 107, 0, 0);
      draw_sprite(temp);
      draw_score(hs->scores[i], i * 50 + 107);
      free(temp);
    }
  }

  if (hs->back_button->highlighted)
    draw_sprite(hs->back_button->highlighted_map);
  else
    draw_sprite(hs->back_button->default_map);

  draw_sprite(hs->title);
  draw_sprite(hs->rank_col);
  draw_sprite(hs->name_col);
  draw_sprite(hs->time_col);
  draw_sprite(hs->date_col);
  draw_sprite(hs->cursor);
}

int is_highscore(highscores_t *hs, uint16_t time_played) {
  int8_t i = MAX_SCORES - 1;
  for (; i >= 0; i--) {
    if (hs->scores[i] == NULL)
      continue;
    else if (hs->scores[i]->time_played <= time_played && i == MAX_SCORES - 1)
      return 0;
    if (hs->scores[i]->time_played <= time_played) {
      last_index = i + 1;
      return 1;
    }
  }

  last_index = 0;
  return 1;
}

void insert_highscore(highscores_t *hs, score_info_t *scr) {
  deleted_score = hs->scores[MAX_SCORES - 1];

  for (uint8_t i = MAX_SCORES - 1; i > last_index; i--) {
    hs->scores[i] = hs->scores[i - 1];
  }

  hs->scores[last_index] = scr;
}

void cancel_highscore(highscores_t *hs) {
  if (!hs->scores[last_index]->done) {
    for (uint8_t i = last_index; i < MAX_SCORES - 1; i++) {
      hs->scores[i] = hs->scores[i + 1];
    }

    hs->scores[MAX_SCORES - 1] = deleted_score;

    last_index = -1;
  }
}
