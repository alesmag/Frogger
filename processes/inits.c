#include "utils/utils.h"

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

void init_high_grass(high_grass *hg)
{
  hg->grass = init(START_TX, START_TY, FROG_HEIGHT + 1, false, '/');
  hg->num_nest = NUM_NEST;
}

void create_nest(high_grass *hg)
{
  int i, j, space;

  space = FROG_LEN + 2;
  j = FROG_LEN;

  // inizializza la lunghezza di ogni tana
  for(i = 0; i < hg->num_nest; i++)
  {
    hg->nest[i] = init_object(j, hg->grass.pos.y + 1, space, ' ', i, false, NEG_VAL, N);
    j += space * 2;
  }
}

void init_river(lane_area *river, high_grass hg)
{
  river->num_lines = NUM_MIN_LINE + rand() % (NUM_MAX_LINE + 1 - NUM_MIN_LINE);

  // una linea viene considerata alta due (ovvero quanto la rana)
  river->area = init(hg.grass.pos.x, hg.grass.pos.y + hg.grass.height, river->num_lines * FROG_HEIGHT, false, '_');
}

void init_road(lane_area *road, game_area grass)
{
  road->num_lines = NUM_MIN_LINE + rand() % (NUM_MAX_LINE + 1 - NUM_MIN_LINE);

  // una linea viene considerata alta due (ovvero quanto la rana)
  road->area = init(grass.pos.x, grass.pos.y + grass.height, road->num_lines * FROG_HEIGHT, true, '_');
}

void init_table(table *t)
{
  init_river(&t->river, t->hg);
  t->grass = init(t->river.area.pos.x, t->river.area.pos.y + t->river.area.height, FROG_HEIGHT, true, '/');
  init_road(&t->road, t->grass);
  t->start = init(t->road.area.pos.x, t->road.area.pos.y + t->road.area.height, FROG_HEIGHT, true, '/');
  
  len_yw = t->start.pos.y + t->start.height;
}

object init_object(int x, int y, int len, char c, int id, bool is_full, pid_t pid, DIR direction)
{
  object obj;

  obj.pos.x = x;
  obj.pos.y = y;
  obj.len = len;
  obj.c = c;
  obj.id = id;
  obj.is_full = is_full;
  obj.pid = pid;
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

void init_pipe(int fd[])
{
  if(pipe(fd) == NEG_VAL) 
  {
    perror("Pipe Error");
    _exit(1);
  }
}