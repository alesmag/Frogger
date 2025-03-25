#include "utils/utils.h"

extern table t;
extern int score, vite;

void draw_high_grass(WINDOW *win)
{
  int i, j, k = 0, g;

  wattrset(win, COLOR_PAIR(BLACK_GREEN));

  for(i = t.hg.grass.pos.y; i < t.hg.grass.pos.y + (t.hg.grass.height - FROG_HEIGHT); i++)
    for(j = t.hg.grass.pos.x; j < t.hg.grass.pos.x + LEN_XT; j++)
      mvwaddch(win, i, j, t.hg.grass.texture);

  i = 0;

  for(j = t.hg.grass.pos.x; j < t.hg.grass.pos.x + LEN_XT; j++)
  {
    if(j == t.hg.nest[k].pos.x)
    {
      for(g = 0; g < t.hg.nest[k].len; g++)
      {
        wattrset(win, COLOR_PAIR(BLACK_BLACK));
        mvwaddch(win, t.hg.grass.pos.y + i + 1, j, t.hg.nest[k].c);
        j++;
      }

      k++;
    }

    else 
    {
      wattrset(win, COLOR_PAIR(BLACK_GREEN));
      mvwaddch(win, t.hg.grass.pos.y + i + 1, j, t.hg.grass.texture);
      mvwaddch(win, t.hg.grass.pos.y + i + 2, j, t.hg.grass.texture);
    }

    if(j == t.hg.grass.pos.x + LEN_XT)
    {
      i++;
      j = t.hg.grass.pos.x;
    }

    if(i == t.hg.grass.height - 1)
      break;
  }
}

void draw_area(WINDOW *win, game_area area, int colour_id)
{
  int i, j;

  wattrset(win, COLOR_PAIR(colour_id));

  for(i = area.pos.y; i < area.pos.y + area.height; i++)
    for(j = area.pos.x; j < area.pos.x + LEN_XT; j++)
      mvwaddch(win, i, j, area.texture);
}

void draw_zone(WINDOW *win, coord pos, int len, int height, char c, int colour_id)
{
  int i, j;

  wattrset(win, COLOR_PAIR(colour_id));

  for(i = pos.y; i < pos.y + height; i++)
    for(j = pos.x; j < pos.x + len; j++)
      mvwaddch(win, i, j, c);
}

void draw_object(WINDOW *win, object obj)
{
  int i;

  switch(obj.c)
  {
    case FROG_C:
      wattrset(win, COLOR_PAIR(BLACK_YELLOW));
      mvwaddstr(win, obj.pos.y, obj.pos.x, "'^' ");
      mvwaddstr(win, obj.pos.y + 1, obj.pos.x, "_()_");
    break;
  
    case LOG_C:
      wattrset(win, COLOR_PAIR(BLACK_MAG));

      mvwaddstr(win, obj.pos.y, obj.pos.x, "|");
      for(i = 1; i <= obj.len - 2; i++)
        mvwaddstr(win, obj.pos.y, obj.pos.x + i, "'");
      mvwaddstr(win, obj.pos.y, obj.pos.x + i, "|");

      mvwaddstr(win, obj.pos.y + 1, obj.pos.x, "|");
      for(i = 1; i <= obj.len - 2; i++)
        mvwaddstr(win, obj.pos.y + 1, obj.pos.x + i, "_");
      mvwaddstr(win, obj.pos.y + 1, obj.pos.x + i, "|");
    break;
    
    case CAR_C:
      wattrset(win, COLOR_PAIR(BLACK_CYAN));

      mvwaddstr(win, obj.pos.y, obj.pos.x, "<");
      for(i = 1; i <= obj.len - 2; i++)
        mvwaddstr(win, obj.pos.y, obj.pos.x + i, "_");
      mvwaddstr(win, obj.pos.y, obj.pos.x + i, ">");

      mvwaddstr(win, obj.pos.y + 1, obj.pos.x, " ");
      for(i = 1; i <= obj.len - 2; i++)
        mvwaddstr(win, obj.pos.y + 1, obj.pos.x + i, "O");
      mvwaddstr(win, obj.pos.y + 1, obj.pos.x + i, " ");
    break;

    case TIMER_C:
      wattrset(win, COLOR_PAIR(BLACK_BLACK));
      mvwaddstr(win, obj.pos.y, obj.pos.x, "TIME: # ");
      mvwprintw(win, obj.pos.y, obj.pos.x + obj.len, "%d", obj.id);
    break;

    case ENEMY_C:
      wattrset(win, COLOR_PAIR(WHITE_BLACK));
      mvwaddstr(win, obj.pos.y, obj.pos.x, "*^* ");
      mvwaddstr(win, obj.pos.y + 1, obj.pos.x, "_[]_");
    break;

    case BULLET_C:
      wattrset(win, COLOR_PAIR(BLACK_WHITE));
      mvwaddstr(win, obj.pos.y, obj.pos.x, "|");
    break;
    
    case LASER_C:
      wattrset(win, COLOR_PAIR(BLACK_WHITE));
      mvwaddstr(win, obj.pos.y, obj.pos.x, "*");
    break;

    default:
      break;
  }
}

void draw_table(WINDOW *bg)
{
  draw_high_grass(bg);
  draw_area(bg, t.river.area, WHITE_BLUE);
  draw_area(bg, t.grass, BLACK_GREEN);
  draw_area(bg, t.road.area, WHITE_BLACK);
  draw_area(bg, t.start, BLACK_GREEN);
}

void reset_table(WINDOW *win, object obj, int height)
{
  int i, j;

  if((obj.pos.y < t.river.area.pos.y + t.river.area.height) && (obj.pos.y >= t.river.area.pos.y))
    draw_zone(win, obj.pos, obj.len, height, t.river.area.texture, WHITE_BLUE);

  else if((obj.pos.y < t.grass.pos.y + t.grass.height) && (obj.pos.y >= t.grass.pos.y))
    draw_zone(win, obj.pos, obj.len, height, t.grass.texture, BLACK_GREEN);

  else if ((obj.pos.y < t.road.area.pos.y + t.road.area.height) && (obj.pos.y >= t.road.area.pos.y))
    draw_zone(win, obj.pos, obj.len, height, t.road.area.texture, WHITE_BLACK);

  else if((obj.pos.y < t.start.pos.y + t.start.height) && (obj.pos.y >= t.start.pos.y))
    draw_zone(win, obj.pos, obj.len, height, t.start.texture, BLACK_GREEN);

  curs_set(0);
}

void draw_frog_in_nest(WINDOW *win)
{
  object frog; 

  for(int i = 0; i < NUM_NEST; i++)
  {
    if(t.hg.nest[i].is_full == true)
    {
      frog = init_object(t.hg.nest[i].pos.x + 1, t.hg.nest[i].pos.y, FROG_LEN, FROG_C, 0, false, NEG_VAL, N);
      draw_object(win, frog);
    }
  }
}

void draw_stats(WINDOW *win)
{
  wattrset(win, COLOR_PAIR(BLACK_BLACK));
  mvwprintw(win, 0, LEN_XT - 8, "LIVES: %d", vite);
  mvwprintw(win, 0, LEN_XT / 2 - FROG_LEN, "SCORE: %d", score);
  wattroff(win, COLOR_PAIR(BLACK_BLACK));
}