#include "utils/utils.h"

extern table t;
extern int num_log;

void* _log(void* arg)
{
  int id = (int) arg;
  int x = 0, len, lines = 0;
  which_obj(t.river, id, &lines);

  srand(time(NULL));

  len = (FROG_LEN * 2) + rand() % (FROG_LEN * 3 - FROG_LEN * 2);
  x = (t.river.lane_vect[lines].direction == L) ? t.river.area.pos.x : t.river.area.pos.x + LEN_XT - len;
  object log = init_object(x, t.river.area.pos.y + (lines * FROG_HEIGHT), len, LOG_C, id, false, pthread_self(), t.river.lane_vect[lines].direction);
 
  push_obj(log);

  while(1)
  {
    for(int i = 0; i < t.river.lane_vect[lines].speed; i++)
    {
      if((log.pos.x + log.len == LEN_XT + START_TX) && (log.direction == L))
      {
        log.direction = R;
        break;
      }

      else if((log.pos.x  == START_TX) && (log.direction == R)) 
      {
        log.direction = L;
        break;
      }

      else if(log.direction == L)
        log.pos.x += PASSO;

      else if (log.direction == R)
        log.pos.x -= PASSO;
    }

    usleep(LOG_WAIT);
    push_obj(log);
  }
}

void create_logs()
{
  int i;
  DIR direction;

  srand(time(NULL));

  direction = rand() % NUM_DIR;
  t.river.lane_vect = malloc(sizeof(lane) * t.river.num_lines);

  for(i = 0; i < t.river.num_lines; i++)
  {
    t.river.lane_vect[i].num_obj = NUM_LOG_LINE;
    t.river.lane_vect[i].direction = direction;
    t.river.lane_vect[i].speed = MIN_LOG_SPEED + rand() % (MAX_LOG_SPEED - MIN_LOG_SPEED);
    t.river.lane_vect[i].obj = malloc(sizeof(object) * NUM_LOG_LINE);

    if(direction == L)
      direction = R;
    else
      direction = L;
  }
}

void log_process()
{
  int i, j, count;

  pthread_t thread_log[t.river.num_lines * NUM_LOG_LINE];

  i = j = count = 0;
  num_log = t.river.num_lines * NUM_LOG_LINE;
  
  create_logs();
  
  for(count = 0; count < t.river.num_lines * NUM_LOG_LINE; count++)
  {
    pthread_create(&thread_log[count], NULL, _log, count);
    
    j++;

    if(j == NUM_LOG_LINE)
    {
      i++;
      j = 0; 
    }
  }
}

void close_logs(object logs[])
{
  for(int i = 0; i < num_log; i++)
    pthread_cancel(logs[i].th_id);
}
