#include "utils/utils.h"

extern int num_bullet, num_enemy, num_laser;

void* _bullet(void* arg)
{
  coord *pos;
  pos = (coord *) arg;
  object bullet = init_object(pos->x + 1, pos->y - 1, 1, BULLET_C, num_bullet, false, pthread_self(), N);

  push_obj(bullet);

  while(true) 
  {
    usleep(BULLET_WAIT);       
      
    push_obj(bullet);

    bullet.pos.y -= 1;
  }
}

void genera_bullet(object *enemy, int id_log_full[])
{
  pthread_t thread_laser;
  coord pos;

  srand(time(NULL));
  
  if(num_enemy > 0)
  {
    int id_rand = rand() % (num_enemy);
    id_rand = id_log_full[id_rand];

    if(is_in_vect(id_rand, id_log_full, num_enemy)!= NEG_VAL)
    {
      if(id_rand < num_enemy && id_rand >= 0)
      {
        pos.x = enemy[id_rand].pos.x;
        pos.y = enemy[id_rand].pos.y;

        pthread_create(&thread_laser, NULL, _laser, &pos);

        num_laser += 1;
      }
    }
  }
}

void* _laser(void* arg)
{
  coord *pos;
  pos = (coord *) arg;
  object laser = init_object(pos->x + 1, pos->y + 1, 1, LASER_C, num_laser, false, pthread_self(), N);

  push_obj(laser);

  while (true) 
  {
    usleep(BULLET_WAIT);       
      
    push_obj(laser);

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
      pthread_cancel(bl_vect[i].th_id);
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
      pthread_cancel(laser[i].th_id);
      laser[i] = reset_object();
      
      return i;
    }
  }

  return NEG_VAL;
}

void close_bullets(object *bullets) 
{
  for(int i = 0; i < num_bullet; i++)
    if(bullets[i].th_id != NEG_VAL)
      pthread_cancel(bullets[i].th_id);
}

void close_lasers(object *lasers) 
{
  for(int i = 0; i < num_laser; i++)
    if(lasers[i].th_id != NEG_VAL)
      pthread_cancel(lasers[i].th_id);
}