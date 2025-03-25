#include "utils/utils.h"

extern table t;
extern sem_t fullBuff, emptyBuff;
extern int diff, vite, score, max_x;

int main()
{
  WINDOW *bg;
  bool is_over = false;
  pid_t pid;

  initscr();
  noecho();
  curs_set(0);

  consoleSize();

  if(!menu())
  {
    endwin();
    return 0;
  }

  sem_init(&fullBuff, 0, 0);
  sem_init(&emptyBuff, 0, DIM_BUFFER); 

  init_colors();

  while(!is_over)
  {
    init_high_grass();
    create_nest();

    if(diff == NORMAL)
      vite = NUM_VITE;
    else
      vite = NUM_VITE - 1;
      
    score = 0;

    while((vite > 0) && (is_nests_full(t) == false))
    {
      bg = newwin(MIN_Y + 1, LEN_XT, 0, max_x / 4);

      refresh();
      init_table();
      draw_table(bg);
      wrefresh(bg);

      frog_process();
      log_process();
      car_process();    
      timer_process();  

      area_gioco(bg);

      free_lanes(&t);
      free_buffer();
    } 

    is_over = end_box(is_nests_full(t));
    delwin(bg);
  }

  endwin();
  system("clear");

  return 0;
}
