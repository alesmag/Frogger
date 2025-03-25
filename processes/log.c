#include "utils/utils.h"

extern int num_log;

void _log(int pipeOut, object *log, DIR direction, int speed, coord pos, int line, int id)
{
  int x = 0, len;

  srand(time(NULL));

  len = (FROG_LEN * 2) + rand() % (FROG_LEN * 3 - FROG_LEN * 2);
  x = (direction == L) ? pos.x : pos.x + LEN_XT - len;
  *log = init_object(x, pos.y + (line * FROG_HEIGHT), len , LOG_C, id, false, getpid(), direction);
  
  write(pipeOut, log, sizeof(object));

  while(1)
  {
    for(int i = 0; i < speed; i++)
    {
      if((log->pos.x + log->len == LEN_XT + START_TX) && (log->direction == L))
      {
        log->direction = R;
        break;
      }

      else if((log->pos.x  == START_TX) && (log->direction == R)) 
      {
        log->direction = L;
        break;
      }

      else if(log->direction == L)
        log->pos.x += PASSO;

      else if (log->direction == R)
        log->pos.x -= PASSO;
    }

    write(pipeOut, log, sizeof(object));
    usleep(LOG_WAIT);
  }
}

void create_logs(lane_area *river)
{
  int i;
  DIR direction;

  srand(time(NULL));

  direction = rand() % NUM_DIR;
  river->lane_vect = malloc(sizeof(lane) * river->num_lines);

  for(i = 0; i < river->num_lines; i++)
  {
    river->lane_vect[i].num_obj = NUM_LOG_LINE;
    river->lane_vect[i].direction = direction;
    river->lane_vect[i].speed = MIN_LOG_SPEED + rand() % (MAX_LOG_SPEED - MIN_LOG_SPEED);
    river->lane_vect[i].obj = malloc(sizeof(object) * NUM_LOG_LINE);

    if(direction == L)
      direction = R;
    else
      direction = L;
  }
}

void log_process(int fd[], table *t)
{
  int i, j, count;
  pid_t pid_log[t->river.num_lines * NUM_LOG_LINE];

  i = j = count = 0;
  
  create_logs(&t->river);

  num_log = t->river.num_lines * NUM_LOG_LINE;
  
  for(count = 0; count < num_log; count++)
  {
    pid_log[count] = fork();

    if(pid_log[count] < 0)
    {
      perror("Log Fork Call");
      _exit(1);
    }

    else if (pid_log[count] == 0)
    {
      close(fd[READING]);
      _log(fd[WRITING], &t->river.lane_vect[i].obj[j], t->river.lane_vect[i].direction, t->river.lane_vect[i].speed, t->river.area.pos, i, count);
    }

    j++;

    if(j ==  NUM_LOG_LINE)
    {
      i++;
      j = 0; 
    }
  }
}

void close_logs(object logs[])
{
  for(int i = 0; i < num_log; i++)
    kill(logs[i].pid, SIGKILL);
}