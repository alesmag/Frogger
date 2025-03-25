#include "utils.h"

/* GLOBAL VARIABLES */

int max_x,              // Dimensione X del terminale
    max_y,              // Dimensione Y del terminale
    len_yw,             // Lunghezza dell'area iniziale
    num_car,            // Numero di macchine presenti
    num_log,            // Numero di tronchi presenti
    vite,               // Numero di vite
    score,              // Punteggio
    num_enemy,          // Numero di nemici presenti
    num_bullet,         // Numero di proiettili rana presenti
    num_laser,          // Numero di proiettili nemici presenti
    *assigned_lanes,    // Vettore contenente l'ordine in cui le corsie sono state riassegnate
    num_assigned_lanes, // Numero delle corsie riassegnate
    diff;               // Difficolta'
    

void consoleSize()
{
  do
  {
    max_x = getmaxx(stdscr);
    max_y = getmaxy(stdscr);

    if((MIN_X > max_x) || (MIN_Y > max_y))
      mvprintw(0, 0, "\nATTENZIONE! \nLe dimensioni della finestra sono: %i x %i.\n"
              "Le dimensioni ottimali richieste sono: %i x %i.\n\n"
              "Si prega di modificare le dimensioni.", max_x, max_y, MIN_X, MIN_Y);

    refresh();
  } while(MIN_X > max_x || MIN_Y > max_y);

  clear();
  refresh();
}

void timer_process(int fd[])
{
  pid_t pid_timer = fork();

  if(pid_timer < 0)
  {
    perror("Frog Fork Call");
    _exit(1);
  }

  else if(pid_timer == 0)
  {
    close(fd[READING]);
    object timer = init_object(0, 0, TIMER_LEN, TIMER_C, MAX_TIMER, false, getpid(), N);

    while(1)
    {
      if(write(fd[WRITING], &timer, sizeof(object)) < 0)
        _exit(7);

      sleep(1);
      timer.id -= 1;
    }
  }
}

void close_timer(object timer) { kill(timer.pid, SIGKILL); }

void close_all(object timer, object frog, object cars[], object logs[], object bullets[], object lasers[])
{
  close_timer(timer);
  close_frog(frog);
  close_cars(cars);
  close_logs(logs);
  close_bullets(bullets);
  close_lasers(lasers);
}

void free_lanes(table *t)
{
  free(t->road.lane_vect);
  free(t->river.lane_vect);
}

void compact_array(int *vect, int *n)
{
  for(int i = 0; i < *n; i++)
  {
    if(vect[i] == NEG_VAL)
    {
      vect[i] = vect[*n-1];
      break;
    }
  }
  
  *n -= 1;
}

int is_in_vect(int id, int *vect, int n)
{
  for(int i = 0; i < n; i++)
    if(vect[i] == id)
      return i;

  return NEG_VAL;
}

bool is_nests_full(table t)
{
  for(int i = 0; i < NUM_NEST; i++)
    if(t.hg.nest[i].is_full == false)
      return false;
  
  return true;
}