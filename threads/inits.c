#include "utils/utils.h"

extern table t;
extern int len_yw;

game_area init(int x, int y, int h, bool is_walkable, char t)
{
  game_area area;

  area.pos.x = x;
  area.pos.y = y;
  area.height = h;
  area.is_walkable = is_walkable;
  area.texture = t;

  return area;
}

void init_high_grass()
{
  t.hg.grass = init(START_TX, START_TY, FROG_HEIGHT + 1, false, '/');
  t.hg.num_nest = NUM_NEST;
}

void create_nest()
{
  int i, j, space;

  space = FROG_LEN + 2;
  j = FROG_LEN;

  // inizializza la lunghezza di ogni tana
  for(i = 0; i < t.hg.num_nest; i++)
  {
    t.hg.nest[i] = init_object(j, t.hg.grass.pos.y + 1, space, ' ', i, false, NEG_VAL, N);
    j += space * 2;
  }
}

void init_river()
{
  t.river.num_lines = NUM_MIN_LINE + rand() % (NUM_MAX_LINE + 1 - NUM_MIN_LINE);

  // una linea viene considerata alta due (ovvero quanto la rana)
  t.river.area = init(t.hg.grass.pos.x, t.hg.grass.pos.y + t.hg.grass.height, t.river.num_lines * FROG_HEIGHT, false, '_');
}

void init_road()
{
  t.road.num_lines = NUM_MIN_LINE + rand() % (NUM_MAX_LINE + 1 - NUM_MIN_LINE);

  // una linea viene considerata alta due (ovvero quanto la rana)
  t.road.area = init(t.grass.pos.x, t.grass.pos.y + t.grass.height, t.road.num_lines * FROG_HEIGHT, true, '_');
}

void init_table()
{
  init_river();
  t.grass = init(t.river.area.pos.x, t.river.area.pos.y + t.river.area.height, FROG_HEIGHT, true, '/');
  init_road();
  t.start = init(t.road.area.pos.x, t.road.area.pos.y + t.road.area.height, FROG_HEIGHT, true, '/');
  
  len_yw = t.start.pos.y + t.start.height;
}

object init_object(int x, int y, int len, char c, int id, bool is_full, pthread_t th_id, DIR direction)
{
  object obj;

  obj.pos.x = x;
  obj.pos.y = y;
  obj.len = len;
  obj.c = c;
  obj.id = id;
  obj.is_full = is_full;
  obj.th_id = th_id;
  obj.direction = direction;

  return obj;
}

object reset_object()
{
  object obj = init_object(NEG_VAL, NEG_VAL, NEG_VAL, ' ', NEG_VAL, false, NEG_VAL, N);

  return obj;
}

void init_colors()
{
  start_color();
  init_pair(BLACK_BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(BLACK_GREEN, COLOR_BLACK, COLOR_GREEN);
  init_pair(WHITE_BLUE, COLOR_WHITE, COLOR_BLUE);
  init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(BLACK_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(BLACK_MAG, COLOR_BLACK, COLOR_MAGENTA);
  init_pair(BLACK_CYAN, COLOR_BLACK, COLOR_CYAN);
  init_pair(BLACK_YELLOW, COLOR_BLACK, COLOR_YELLOW);
}