#include "utils/utils.h"

extern int num_bullet, num_enemy, num_laser;

void _bullet(int pipeOut, int y, int x)
{
  object bullet;

  bullet = init_object(x + 1, y - 1, 1, BULLET_C, num_bullet, false, getpid(), N);

  if(write(pipeOut, &bullet, sizeof(object)) < 0)
    _exit(3);

  while(true) 
  {
    usleep(BULLET_WAIT);       
      
    if(write(pipeOut, &bullet, sizeof(object)) < 0)
      _exit(3);

    bullet.pos.y -= 1;
  }
}

void genera_bullet(object *enemy, int pipeOut, int id_log_full[])
{
  srand(time(NULL));
  
  if(num_enemy > 0)
  {
    int id_rand = rand() % num_enemy;
    id_rand = id_log_full[id_rand];

    if(id_rand > NEG_VAL)
    {
      pid_t pid_bullet;

      pid_bullet = fork();

      if(pid_bullet < 0)
      {
        perror("Laser Enemy Call");
        _exit(1);
      }

      else if (pid_bullet == 0)
        _laser(pipeOut, enemy[id_rand].pos.y , enemy[id_rand].pos.x);

      num_laser += 1;
    }
  }
}

void _laser(int pipeOut, int y, int x)
{
  object laser;

  laser = init_object(x + 1, y + 1 , 1, LASER_C, num_laser, false, getpid(), N);

  //Scrittura nella pipe delle informazioni del proiettile
  if(write(pipeOut, &laser, sizeof(object)) < 0)
    _exit(3);

  while (true) 
  {
    usleep(BULLET_WAIT);       
      
    if(write(pipeOut, &laser, sizeof(object)) < 0)
      _exit(3);

    laser.pos.y += 1;
  }
}

int bullet_enemy_collision(object *enemy, object bullet, int id_log_full[])
{
  for(int i = 0; i < num_enemy; i++)
  {
    if((bullet.pos.x <= enemy[id_log_full[i]].pos.x + enemy[id_log_full[i]].len) && (bullet.pos.x >= enemy[id_log_full[i]].pos.x) && (bullet.pos.y == enemy[id_log_full[i]].pos.y))
    {
      enemy[id_log_full[i]].pos.x = NEG_VAL;
      enemy[id_log_full[i]].id = NEG_VAL;
      id_log_full[i] = NEG_VAL;
      compact_array(id_log_full, &num_enemy);
      system("aplay AUDIO/enemykilled.wav 2> AUDIO/dump.txt &");

      return i;
    }
  }

  return NEG_VAL;
}

int laser_bullet_collision(object bl_att, object *bl_vect)
{
  for(int i = 0; i < num_laser; i++)
  {
    if((bl_att.pos.x + bl_att.len >= bl_vect[i].pos.x) && (bl_att.pos.x <= bl_vect[i].pos.x) && (bl_att.pos.y == bl_vect[i].pos.y))
    {
      kill(bl_vect[i].pid, SIGKILL);
      bl_vect[i] = reset_object();

      return i;
    }
  }

  return NEG_VAL;
}

int laser_frog_collision(object frog, object *laser)
{
  for(int i = 0; i < num_laser; i++)
  {
    if((frog.pos.x + frog.len >= laser[i].pos.x) && (frog.pos.x <= laser[i].pos.x) && (frog.pos.y == laser[i].pos.y))
    {
      kill(laser[i].pid, SIGKILL);
      laser[i] = reset_object();
      
      return i;
    }
  }

  return NEG_VAL;
}

void close_bullets(object *bullets) 
{
  for(int i = 0; i < num_bullet; i++)
    if(bullets[i].pid > 0)
      kill(bullets[i].pid, SIGKILL);
}

void close_lasers(object *lasers) 
{
  for(int i = 0; i < num_laser; i++)
    if(lasers[i].pid > 0)
      kill(lasers[i].pid, SIGKILL);
}