#include "utils/utils.h"

extern int len_yw, num_bullet;

void _frog(int pipeOut, table t)
{
  object frog = init_object(t.start.pos.x + STEP, t.start.pos.y, FROG_LEN, FROG_C, 0, false, getpid(), N);
  char c;
  pid_t pid_bullet;

  if(write(pipeOut, &frog, sizeof(object)) < 0)
    _exit(3);

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
        pid_bullet = fork();

        if(pid_bullet < 0)
        {
          perror("Bullet Frog Call");
          _exit(1);
        }

        else if (pid_bullet == 0)
        {
          system("aplay AUDIO/shot.wav 2> AUDIO/dump.txt &");
          _bullet(pipeOut, frog.pos.y, frog.pos.x);
        }

        num_bullet += 1;
      break;
    }

    if(write(pipeOut, &frog, sizeof(object)) < 0)
      _exit(3);

    usleep(FROG_WAIT);
  }
}

void frog_process(int fd[], table t)
{
  pid_t pid_frog = fork();

  if(pid_frog < 0)
  {
    perror("Frog Fork Call");
    _exit(1);
  }

  else if(pid_frog == 0)
  {
    close(fd[READING]);
    _frog(fd[WRITING], t);
  }
}

void close_frog(object frog) { kill(frog.pid, SIGKILL); }