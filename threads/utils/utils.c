#include "utils.h"

/* GLOBAL VARIABLES */

table t;                        // Variabile che rappresenta il tavolo da gioco

pthread_mutex_t mutex;          // Mutex

sem_t fullBuff,                 // Semaforo per il buffer pieno
      emptyBuff;                // Semaforo per il buffer vuoto

object buffer[DIM_BUFFER],      // Buffer utilizzato per la comunicazione fra i thread
       taken_obj;               // Ultimo oggetto preso dal buffer

int max_x,                      // Dimensione X del terminale
    max_y,                      // Dimensione Y del terminale
    len_yw,                     // Lunghezza dell'area iniziale
    num_car,                    // Numero di macchine presenti
    num_log,                    // Numero di tronchi presenti
    vite,                       // Numero di vite
    score,                      // Punteggio
    num_enemy,                  // Numero di nemici presenti
    num_bullet,                 // Numero di proiettili rana presenti
    num_laser,                  // Numero di proiettili nemici presenti
    *assigned_lanes,            // Vettore contenente l'ordine in cui le corsie sono state riassegnate
    num_assigned_lanes,         // Numero delle corsie riassegnate
    diff,                       // Difficolta'
    index_add = 0,              // Indice utilizzato per segnalare la prima posizione disponibile del buffer
    index_remove = 0;           // Indice utilizzato per segnalare l'ultima posizione occupata del buffer


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

void timer_process()
{
  pthread_t thread_timer;
  pthread_create(&thread_timer, NULL, _timer, NULL);
}

void* _timer(void* arg)
{
  object timer = init_object(0, 0, TIMER_LEN, TIMER_C, MAX_TIMER, false, pthread_self(), N);

  push_obj(timer);

  while(1)
  {
    sleep(1);
    timer.id -= 1;
    push_obj(timer);
  }
}

void close_timer(object timer) { pthread_cancel(timer.th_id); }

void close_all(object timer, object frog, object cars[], object logs[], object bullets[], object lasers[])
{
  close_timer(timer);
  close_frog(frog);
  close_cars(cars);
  close_logs(logs);
  close_bullets(bullets);
  close_lasers(lasers);
}

void free_lanes()
{
  free(t.road.lane_vect);
  free(t.river.lane_vect);
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

bool is_nests_full()
{
  for(int i = 0; i < NUM_NEST; i++)
    if(t.hg.nest[i].is_full == false)
      return false;
  
  return true;
}

object which_obj(lane_area area, int id, int *line)
{
  int count = 0;

  for(int i = 0; i < area.num_lines; i++)
  {
    for(int j = 0; j < area.lane_vect[i].num_obj; j++)
    {
      if(count == id)
      {
        *line = i;
        return area.lane_vect[i].obj[j];
      }

      count++;
    }
  }

  return reset_object();
}

void push_obj(object obj) 
{
  sem_wait(&emptyBuff);
  
  pthread_mutex_lock(&mutex);
  buffer[index_add] = obj;
  index_add = (index_add + 1) % DIM_BUFFER;
  pthread_mutex_unlock(&mutex);
  
  sem_post(&fullBuff);
}

void pop_obj()
{
  sem_wait(&fullBuff);

  pthread_mutex_lock(&mutex);
  taken_obj = buffer[index_remove];
  index_remove = (index_remove + 1) % DIM_BUFFER;
  pthread_mutex_unlock(&mutex);

  sem_post(&emptyBuff);
}

void free_buffer() 
{
  for(int i = 0; i < DIM_BUFFER; i++) 
    buffer[i] = reset_object();
}