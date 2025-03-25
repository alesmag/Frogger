#include "utils/utils.h"

extern int diff, vite, score, max_x;

int main()
{
  table t;
  WINDOW *bg;
  bool is_over = false;

  initscr();
  noecho();
  curs_set(0);

  consoleSize();

  if(!menu())
  {
    endwin();
    return 0;
  }

  init_colors();

  while(!is_over)
  {
    init_high_grass(&t.hg);
    create_nest(&t.hg);

    if(diff == NORMAL)
      vite = NUM_VITE;
    else
      vite = NUM_VITE - 1;
      
    score = 0;

    while((vite > 0) && (is_nests_full(t) == false))
    {
      int fd[FD_SIZE];
      bg = newwin(MIN_Y + 1, LEN_XT, 0, max_x / 4);

      init_pipe(fd);

      init_table(&t);
      draw_table(bg, t);

      frog_process(fd, t);
      log_process(fd, &t);
      car_process(fd, &t);
      timer_process(fd);
      area_gioco(bg, fd, &t);

      free_lanes(&t);
    } 

    is_over = end_box(is_nests_full(t));
    delwin(bg);
  }

  endwin();
  system("clear");

  return 0;
}