#include "utils/utils.h"

extern table t;
extern int len_yw, num_bullet;

void* _frog(void* arg)
{
  pthread_t thread_bullet;
  object frog = init_object(t.start.pos.x + STEP, t.start.pos.y, FROG_LEN, FROG_C, 0, false, pthread_self(), N);
  char c;
  coord pos;

  push_obj(frog);

  while(1)
  {
    frog.direction = N;
    
    switch(c = getch())
    {
      case UP:
        if(frog.pos.y > START_TY + STEP)
        {
          frog.pos.y -= STEP;
          frog.direction = N;
        }
      break;
      
      case DOWN:
        if(frog.pos.y < len_yw - STEP)
        {
          frog.pos.y += STEP;
          frog.direction = N;
        }
      break;
      
      case RIGHT:
        frog.pos.x += STEP;
        frog.direction = L;
      break;
      
      case LEFT:
        frog.pos.x -= STEP;
        frog.direction = R;
      break;
      
      case SPACE:
        system("aplay AUDIO/shot.wav 2> AUDIO/dump.txt &");
        pos.x = frog.pos.x;
        pos.y = frog.pos.y;

        pthread_create(&thread_bullet, NULL, _bullet, &pos);
        num_bullet += 1;
      break;
    }

    push_obj(frog);
  }

  push_obj(frog);
}

void frog_process()
{
  pthread_t thread_frog;
  pthread_create(&thread_frog, NULL, _frog, NULL);
}

void close_frog(object frog) { pthread_cancel(frog.th_id); }