#include "logic.h"
#include "serialPort.h"
#include <lcom/lcf.h>
extern uint16_t time_counter;

static game_state_t main_state = main_menu;

static game_t *game;
static menu_t *menu;
static highscores_t *hs_menu;

static uint16_t year;
static uint8_t month;
static uint8_t day;
static uint8_t connection_flag = 0;

void initialize_game() { game = new_game(); }

void initialize_menu() { menu = create_menu(); }

void initialize_highscores() { hs_menu = create_highscores(); }

void initialize_date(uint8_t y, uint8_t m, uint8_t d) {
  year = y + 1993;
  month = m;
  day = d;
}

void delete_all() {
  delete_menu(menu);
  delete_game(game);
  delete_highscores(hs_menu);
}

void update_game_state(int state) {
  main_state = state;
  vg_clear_display();

  if (main_state == main_menu) {
    Sprite *background = create_sprite_direct(background_xpm, 0, 0, 0, 0);
    load_background(background);
    free(background);
    draw_menu(menu);
  }
  else if (main_state == playing_local || main_state == playing_remote_host || main_state == playing_remote_guest) {
    draw_static(game);
    draw_boosts(game);
    draw_points(game);
    draw_time(game);
  }
  else if (main_state == high_score) {
    Sprite *hs_background = create_sprite_direct(hs_background_xpm, 0, 0, 0, 0);
    load_background(hs_background);
    free(hs_background);
    draw_hs(hs_menu);
  }
}

int kbd_logic(event_keyboard *p) {

  switch (main_state) {

    case main_menu:
      break;

    case playing_local:

      switch (*p) {
        // player 1 keys
        case W_pressed:
          set_direction(game, true, 'N');
          break;

        case A_pressed:
          set_direction(game, true, 'W');
          break;

        case D_pressed:
          set_direction(game, true, 'E');
          break;

        case S_pressed:
          set_direction(game, true, 'S');
          break;

        case space_pressed:
          boost_trigger(game, true);
          break;

          // player 2 keys

        case up_pressed:
          set_direction(game, false, 'N');
          break;

        case down_pressed:
          set_direction(game, false, 'S');
          break;

        case left_pressed:
          set_direction(game, false, 'W');
          break;

        case right_pressed:
          set_direction(game, false, 'E');
          break;

        case enter_pressed:
          boost_trigger(game, false);
          break;

          // global keys

        case ESC_pressed:
          game_runner(game);
          if (game->running)
            return 1; //game has started, needs to activate rtc periodic interrupts
          else
            return 2; //game is paused, rtc interrupts must be disabled
          break;
        default:
          break;
      }
      break;

    case high_score:
      if (valid_hs_op()) {
        if (*p == ESC_pressed) {
          cancel_highscore(hs_menu);
          draw_hs(hs_menu);
        }
        else if (*p == enter_pressed) {
          finish_score(hs_menu);
        }
      }
      break;

    case playing_remote_guest:
      switch (*p) {
        case W_pressed:
          uart_write_message(PROT_UP_EVENT);
          set_direction(game, false, 'N');
          break;

        case A_pressed:
          uart_write_message(PROT_LEFT_EVENT);
          set_direction(game, false, 'W');
          break;

        case D_pressed:
          uart_write_message(PROT_RIGHT_EVENT);
          set_direction(game, false, 'E');
          break;

        case S_pressed:
          uart_read_data();
          uart_write_message(PROT_DOWN_EVENT);
          set_direction(game, false, 'S');
          break;
        default:
          break;
      }
      break;

    case playing_remote_host:
      switch (*p) {
        case W_pressed:
          uart_write_message(PROT_UP_EVENT);
          set_direction(game, true, 'N');
          break;

        case A_pressed:
          uart_write_message(PROT_LEFT_EVENT);
          set_direction(game, true, 'W');
          break;

        case D_pressed:
          uart_write_message(PROT_RIGHT_EVENT);
          set_direction(game, true, 'E');
          break;

        case S_pressed:
          uart_write_message(PROT_DOWN_EVENT);
          set_direction(game, true, 'S');
          break;

        case ESC_pressed:
          if (connection_flag) {
            uart_write_message(PROT_ROUND_START);
            game_runner(game);
            if (game->running)
              return 1; //game has started, needs to activate rtc periodic interrupts
            else
              return 2; //game is paused, rtc interrupts must be disabled
          }
          break;
        default: break;
      }

    default:
      break;
  }
  return 0;
}

int timer_logic() {
  pageFlip();
  static bool invert_draw = false;
  round_winner_t winner = no_winner;

  switch (main_state) {
    case main_menu:
      draw_coin(menu);
      break;

    case playing_local:

      if (game_tick(game) == 1) {
        return 0; //game is paused or hasn't started
      }

      if (invert_draw) {
        if (draw_trail(game->p2) != 0) {
          winner = player1;
        }
        if (draw_trail(game->p1) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player2;
        }
      }
      else {
        if (draw_trail(game->p1) != 0) {
          winner = player2;
        }

        if (draw_trail(game->p2) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player1;
        }
      }

      if (winner) {
        if (end_round(game, &winner)) {
          uint16_t posx = 0, posy = 0;
          switch (winner) {
            case player1:
              posx = 178;
              posy = 177;
              break;
            case player2:
              posx = 158;
              posy = 177;
              break;
            case draw:
              posx = 237;
              posy = 220;
              break;
            default:
              break;
          }
          Sprite *message = create_sprite_direct(wins[winner - 1], posx, posy, 0, 0);
          draw_sprite(message);
          free(message);
          pageFlip();
          sleep(2);
          if (winner != draw && is_highscore(hs_menu, time_counter / 2)) {
            insert_highscore(hs_menu, create_score(time_counter / 2, year, month, day));
            time_counter = 0;
            update_game_state(high_score);
          }
          else {
            time_counter = 0;
            update_game_state(main_menu);
          }
        }
        return 1;
      }

      invert_draw = !invert_draw;
      break;

    case playing_remote_guest:
      if (game_tick(game) == 1) {
        return 0; //game is paused or hasn't started
      }

      if (invert_draw) {
        if (draw_trail(game->p2) != 0) {
          winner = player1;
        }
        if (draw_trail(game->p1) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player2;
        }
      }
      else {
        if (draw_trail(game->p1) != 0) {
          winner = player2;
        }

        if (draw_trail(game->p2) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player1;
        }
      }

      if (winner) {
        if (end_round(game, &winner)) {
          uint16_t posx = 0, posy = 0;
          switch (winner) {
            case player1:
              posx = 178;
              posy = 177;
              break;
            case player2:
              posx = 158;
              posy = 177;
              break;
            case draw:
              posx = 237;
              posy = 220;
              break;
            default:
              break;
          }
          Sprite *message = create_sprite_direct(wins[winner - 1], posx, posy, 0, 0);
          draw_sprite(message);
          free(message);
          pageFlip();
          connection_flag = 0;
          sleep(2);
          if (winner == player2 && is_highscore(hs_menu, time_counter / 2)) {
            insert_highscore(hs_menu, create_score(time_counter / 2, year, month, day));
            time_counter = 0;
            update_game_state(high_score);
          }
          else {
            time_counter = 0;
            update_game_state(main_menu);
          }
        }
        return 1;
      }

      invert_draw = !invert_draw;
      break;

    case playing_remote_host:
      if (game_tick(game) == 1) {
        return 0; //game is paused or hasn't started
      }

      if (invert_draw) {
        if (draw_trail(game->p2) != 0) {
          winner = player1;
        }
        if (draw_trail(game->p1) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player2;
        }
      }
      else {
        if (draw_trail(game->p1) != 0) {
          winner = player2;
        }

        if (draw_trail(game->p2) != 0) {
          if (winner)
            winner = draw;
          else
            winner = player1;
        }
      }

      if (winner) {
        if (end_round(game, &winner)) {
          uint16_t posx = 0, posy = 0;
          switch (winner) {
            case player1:
              posx = 178;
              posy = 177;
              break;
            case player2:
              posx = 158;
              posy = 177;
              break;
            case draw:
              posx = 237;
              posy = 220;
              break;
            default:
              break;
          }
          Sprite *message = create_sprite_direct(wins[winner - 1], posx, posy, 0, 0);
          draw_sprite(message);
          free(message);
          pageFlip();
          connection_flag = 0;
          sleep(2);
          if (winner == player1 && is_highscore(hs_menu, time_counter / 2)) {
            insert_highscore(hs_menu, create_score(time_counter / 2, year, month, day));
            time_counter = 0;
            update_game_state(high_score);
          }
          else {
            time_counter = 0;
            update_game_state(main_menu);
          }
        }
        return 1;
      }

      invert_draw = !invert_draw;
      break;

    default:
      break;
  }
  return 0;
}

int mouse_logic(struct mouse_ev *ev) {
  switch (main_state) {
    case main_menu:
      if (ev->type == MOUSE_MOV) {
        move_cursor(menu, ev->delta_x, ev->delta_y);

        draw_menu(menu);
      }
      else if (ev->type == LB_RELEASED) {
        click_t but_click = menu_assert_click(menu);

        switch (but_click) {
          case no_click:
            break;

          case local_click:
            if (coins_left(menu)) {
              remove_coin(menu);
              update_game_state(playing_local);
            }
            break;

          case remote_click:
            if (coins_left(menu)) {
              remove_coin(menu);
              if (connection_flag == 1) {
                update_game_state(playing_remote_guest);
                uart_write_message(PROT_CONNECT);
                printf("guest\n");
              }
              else {
                update_game_state(playing_remote_host);
                uart_write_message(PROT_CONNECT);
                printf("host\n");
              }
            }
            break;

          case hs_click:
            update_game_state(high_score);
            break;

          case exit_click:
            load_background(create_sprite_direct(credits_xpm, 0, 0, 0, 0));
            draw_background();
            pageFlip();
            sleep(3);
            return 1;
            break;
        }
      }
      else if (ev->type == RB_RELEASED) {
        add_coin(menu);
      }

      break;

    case playing_local:
      break;

    case playing_remote_host:
      if (ev->type == LB_RELEASED) {
        uart_write_message(PROT_BOOST_EVENT);
        boost_trigger(game, true);
      }
      break;

    case playing_remote_guest:
      if (ev->type == LB_RELEASED) {
        uart_write_message(PROT_BOOST_EVENT);
        boost_trigger(game, false);
      }
      break;

    case high_score:
      if (ev->type == MOUSE_MOV) {
        hs_move_cursor(hs_menu, ev->delta_x, ev->delta_y);

        draw_hs(hs_menu);
      }
      else if (ev->type == LB_RELEASED) {
        hover_digit_t hov = digit_hover(hs_menu->cursor);
        if (hs_assert_click(hs_menu)) {
          if (valid_hs_op())
            finish_score(hs_menu);

          update_game_state(main_menu);
        }
        else if (hov && valid_hs_op()) {
          edit_name(hs_menu, hov, true);
          draw_hs(hs_menu);
        }
      }
      else if (ev->type == RB_RELEASED && valid_hs_op()) {
        hover_digit_t hov = digit_hover(hs_menu->cursor);
        if (hov) {
          edit_name(hs_menu, hov, false);
          draw_hs(hs_menu);
        }
      }
      break;
    default:
      break;
  }

  return 0;
}

int serial_logic(event_serial event) {
  printf("Event:%d\n", event);

  //HOST IS PLAYER 1 ! , REMOTE IS PLAYER 2
  switch (main_state) {

    case playing_remote_host:
      switch (event) {
        case up_key:
          set_direction(game, false, 'N');
          break;
        case down_key:
          set_direction(game, false, 'S');
          break;
        case right_key:
          set_direction(game, false, 'E');
          break;
        case left_key:
          set_direction(game, false, 'W');
          break;
        case boost_key:
          boost_trigger(game, false);
          break;
        case connection_byte:
          uart_write_message(PROT_ACK_BYTE);
          connection_flag = 1;
          break;

        default:
          break;
      }
      break;

    case playing_remote_guest:
      switch (event) {
        case up_key:
          set_direction(game, true, 'N');
          break;
        case down_key:
          set_direction(game, true, 'S');
          break;
        case right_key:
          set_direction(game, true, 'E');
          break;
        case left_key:
          set_direction(game, true, 'W');
          break;
        case boost_key:
          boost_trigger(game, true);
          break;
        case round_start:
          game_runner(game);
          if (game->running)
            return 1; //game has started, needs to activate rtc periodic interrupts
          else
            return 2; //game is paused, rtc interrupts must be disabled
          break;
        default:
          break;
      }

      break;
    case main_menu:
      if (event == connection_byte) {
        connection_flag = 1;
      }
      break;

    default:
      break;
  }
  return 0;
}
