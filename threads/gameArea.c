#include "utils/utils.h"

extern table t;
extern int num_log, num_car, vite, score, diff, num_enemy, num_laser;
extern object taken_obj;

void area_gioco(WINDOW *win)
{
  // Dichiarazione variabili locali
  FROG_STATE state = RUNNING;
  bool on = false;
  coord frog_curr_pos;

  object frog, 
        readVal, 
        log[num_log], 
        car[num_car], 
        timer, 
        enemy[num_log], 
        bullet[MAX_BULLET], 
        laser[MAX_BULLET];

  int pos_frog_log = NEG_VAL, 
      vite_att = vite, 
      id_log_frog_on = NEG_VAL, 
      temp_timer = MAX_TIMER, 
      id_log_full[num_log];

  // Inizializzazione variabili
  frog_curr_pos.x = NEG_VAL;
  frog_curr_pos.y = NEG_VAL;

  frog.pos.y = t.start.pos.y;
  frog.pos.x = NEG_VAL;

  timer.id = MAX_TIMER;

  for(int i = 0; i < num_log; i++)
  {
    log[i] = reset_object();
    enemy[i] = reset_object();
    id_log_full[i] = NEG_VAL;
  }

  for(int i = 0; i < num_car; i++)
    car[i] = reset_object();

  for(int i = 0; i < MAX_BULLET; i++)
  {
    bullet[i] = reset_object();
    laser[i] = reset_object();
  }

  while(timer.id > 0)
  {
    pop_obj();
    readVal = taken_obj;

    draw_stats(win);
    draw_frog_in_nest(win);
    
    // Sezione Frog
    if(readVal.c == FROG_C)
    {
      if((frog.pos.x + (readVal.pos.x - frog_curr_pos.x) <= LEN_XT + START_TX - FROG_LEN) && (frog.pos.x + (readVal.pos.x - frog_curr_pos.x) >= START_TX))
      {
        if(readVal.pos.y > frog.pos.y)
          score -= STEP_SCORE;

        else if(readVal.pos.y < frog.pos.y)
          score += STEP_SCORE;

        if(frog.pos.x >= 0)
          reset_table(win, frog, FROG_HEIGHT);
      
        if(id_log_frog_on > NEG_VAL)
        {
          on = true;
          draw_object(win, log[id_log_frog_on]);
        }

        if(on == true)
        {
          frog.pos.x += readVal.pos.x - frog_curr_pos.x;
          frog_curr_pos.x = readVal.pos.x;
          frog.pos.y = readVal.pos.y;
        }

        else 
        {
          frog_curr_pos = frog.pos;
          frog = readVal;
        }

        curs_set(0);
        draw_object(win, frog);
        wrefresh(win); 
      }
    }
    
    // Sezione Log
    else if(readVal.c == LOG_C)
    {
      if(log[readVal.id].pos.x >= 0)
        reset_table(win, log[readVal.id], FROG_HEIGHT);
      
      log[readVal.id] = readVal; 

      curs_set(0);
      draw_object(win, readVal);

      if(id_log_frog_on == readVal.id)
      {
        frog.pos.x = log[readVal.id].pos.x + pos_frog_log;
        frog.pos.y = log[readVal.id].pos.y;
   
        curs_set(0);
        draw_object(win, frog);
      }

      else if(is_in_vect(readVal.id, id_log_full, num_enemy) != NEG_VAL)
      {
        enemy[readVal.id] = init_object(log[readVal.id].pos.x + 1,log[readVal.id].pos.y, FROG_LEN, ENEMY_C, readVal.id, false, NEG_VAL, N);
        curs_set(0);
        draw_object(win, enemy[readVal.id]);
      }

      wrefresh(win);
    }
    
    // Sezione Car
    else if(readVal.c == CAR_C)
    {
      if(car[readVal.id].pos.x >= 0)
        reset_table(win, car[readVal.id], FROG_HEIGHT);
      
      car[readVal.id] = readVal; 

      curs_set(0);
      draw_object(win, readVal);
      wrefresh(win); 
    }

    else if(readVal.c == TIMER_C)
    {
      timer = readVal;

      curs_set(0);
      draw_object(win, readVal);
      wrefresh(win); 
    }

    // Sezione Proiettile Frog
    else if(readVal.c == BULLET_C)
    {
      if(bullet[readVal.id].pos.x >= 0)
        reset_table(win, bullet[readVal.id], 1);
          
      if(bullet_enemy_collision(enemy, bullet[readVal.id], id_log_full) != NEG_VAL )
      {
        score += ENEMY_SCORE;
        pthread_cancel(bullet[readVal.id].th_id);
        bullet[readVal.id] = reset_object();
      }

      else if(laser_bullet_collision(bullet[readVal.id], laser) != NEG_VAL)
      {
        pthread_cancel(bullet[readVal.id].th_id);
        bullet[readVal.id] = reset_object();
      }
    
      else if(car_collision(t.road.area, car, num_car, bullet[readVal.id], &state) != NEG_VAL)
      {
        pthread_cancel(bullet[readVal.id].th_id);
        bullet[readVal.id] = reset_object();
      }

      else if((bullet[readVal.id].pos.y < t.hg.grass.pos.y + t.hg.grass.height + 1) && (bullet[readVal.id].id != NEG_VAL) && (bullet[readVal.id].pos.x != NEG_VAL))
      {
        pthread_cancel(bullet[readVal.id].th_id);
        bullet[readVal.id] = reset_object();
      }
        
      else
      {
        if(bullet[readVal.id].pos.x == NEG_VAL)
        {
          bullet[readVal.id] = readVal;
      
          if(on == true) 
            bullet[readVal.id].pos.x = frog.pos.x;
        }

        bullet[readVal.id].pos.y = readVal.pos.y;

        curs_set(0);
        draw_object(win, bullet[readVal.id]);
        wrefresh(win); 
      }
    }

    // Sezione Proiettile Nemico
    else if(readVal.c == LASER_C)
    {
      if(laser[readVal.id].pos.x >= 0)
        reset_table(win, laser[readVal.id], 1);

      else if(laser_bullet_collision(laser[readVal.id], bullet) != NEG_VAL)
      {
        pthread_cancel(laser[readVal.id].th_id);
        laser[readVal.id] = reset_object();
      }
    
      if(car_collision(t.road.area, car, num_car, laser[readVal.id], &state) != NEG_VAL)
      {
        pthread_cancel(laser[readVal.id].th_id);
        laser[readVal.id] = reset_object();
      }

      else if((laser[readVal.id].pos.y > t.start.pos.y) && (laser[readVal.id].id != NEG_VAL) && (laser[readVal.id].pos.x != NEG_VAL))
      {
        pthread_cancel(laser[readVal.id].th_id);
        laser[readVal.id] = reset_object();
      }

      else
      {
        if(laser[readVal.id].pos.x == NEG_VAL)
          laser[readVal.id] = readVal;

        laser[readVal.id].pos.y = readVal.pos.y;

        curs_set(0);
        draw_object(win, laser[readVal.id]);
        wrefresh(win); 
      }
    }

    // Collisione con auto
    if(car_collision(t.road.area, car, num_car, frog, &state) != NEG_VAL)
    {
      vite--;
      system("aplay AUDIO/carcrash.wav 2> AUDIO/dump.txt &");
      break;
    }

    // Collisione sezione log
    if(log_collision(t.river.area, log, num_log, frog, &state, id_log_full) != NEG_VAL)
    {
      if(state == HIT)
      {
        vite--;
        break;
      }

      else if(state == ON_OBJ)
      {
        id_log_frog_on = log_collision(t.river.area, log, num_log, frog, &state, id_log_full);
        pos_frog_log = frog.pos.x - log[id_log_frog_on].pos.x;
      }

      else if(state == KILLED)
      {
        vite--;
        system("aplay AUDIO/ouch.wav 2> AUDIO/dump.txt &");
        break;
      }
    }
    
    // Collisione con l'acqua
    else if(state == FELL)
    {
      vite--;
      system("aplay AUDIO/splash.wav 2> AUDIO/dump.txt &");
      break;
    }

    else 
    {
      id_log_frog_on = NEG_VAL;
      pos_frog_log = NEG_VAL;
    }
    
    // Collisione per l'interno di una tana
    if(nest_collision(t.hg.grass, t.hg.nest, t.hg.num_nest, frog, &state) != NEG_VAL)
    {
      if(state == IN_NEST)
      {
        if(t.hg.nest[nest_collision(t.hg.grass, t.hg.nest, t.hg.num_nest, frog, &state)].is_full == true)
        {
          state = NEST_FULL;
          vite--;
          system("aplay AUDIO/missed.wav 2> AUDIO/dump.txt &");
          break;
        }

        else
        {
          t.hg.nest[nest_collision(t.hg.grass, t.hg.nest, t.hg.num_nest, frog, &state)].is_full = true;
          score += NEST_SCORE;
          system("aplay AUDIO/nest.wav 2> AUDIO/dump.txt &");

          score += timer.id * TIME_SCORE;
          break;
        }
      }
    }

    // Collisione con proiettile nemico
    if(laser_frog_collision(frog, laser) != NEG_VAL)
    {
      state = KILLED;
      vite--;
      system("aplay AUDIO/hit.wav 2> AUDIO/dump.txt &");
      break;
    }

    else if(state == FELL)
    {
      vite--;
      break;
    }
    
    // Generazione nemici e proiettili dopo un lasso di tempo
    if(timer.id < temp_timer)
    {
      srand(time(NULL));
      int num_rand = 1 + rand() % (NUM_MIN_LINE - 1);

      for(int i = 0; i < num_rand; i++)
        genera_nemici(enemy, log, id_log_frog_on, id_log_full);
    
      if(num_enemy > 0)
      {
        num_rand = 1 + rand() % (num_enemy * 2);

        for(int i = 0; i < num_rand; i++)
          genera_bullet(enemy, id_log_full);
      }

      if(diff == NORMAL)
        temp_timer -= PAUSE_CREATE_ENEMY;
      else
        temp_timer -= PAUSE_CREATE_ENEMY / 2;
    }
  }

  // Caso in cui il tempo finisca
  if(timer.id == 0)
    vite--;
  
  usleep(DEATH_WAIT);

  close_all(timer, frog, car, log, bullet, laser);

  num_car = num_log = num_enemy = num_laser = 0;
}

int log_collision(game_area area, object *obj, int num, object obj_check, FROG_STATE *state, int id_log_full[])
{
  bool in_area = false; 
  
  if((obj_check.pos.y < area.pos.y + area.height) && (obj_check.pos.y >= area.pos.y))
  {
    in_area = true;

    for(int i = 0; i < num; i++)
    {
      if((obj_check.pos.x + obj_check.len <= obj[i].pos.x + obj[i].len) && (obj_check.pos.x >= obj[i].pos.x) && obj[i].pos.y == obj_check.pos.y)
      {
        if(is_in_vect(i, id_log_full, num_enemy) != NEG_VAL)
          *state = KILLED;
        else 
          *state = ON_OBJ;

        return i; 
      }
    }
  }
  
  if((in_area == true) && (area.is_walkable == false))
    *state = FELL;

  return NEG_VAL; 
}

int car_collision(game_area area, object *obj, int num, object obj_check, FROG_STATE *state)
{ 
  if((obj_check.pos.y < area.pos.y + area.height) && (obj_check.pos.y >= area.pos.y))
  {
    for(int i = 0; i < num; i++)
    {
      if((obj_check.pos.x + obj_check.len > obj[i].pos.x) && (obj_check.pos.x < obj[i].pos.x + obj[i].len)&& obj[i].pos.y == obj_check.pos.y)
      {
        *state = HIT;
        return i;
      }
    }
  }

  return NEG_VAL; 
}

int nest_collision(game_area area, object *obj, int num, object obj_check, FROG_STATE *state)
{
  bool in_area = false;

  if((obj_check.pos.y < area.pos.y + area.height) && (obj_check.pos.y >= area.pos.y))
  {
    in_area = true;

    for(int i = 0; i < num; i++)
    {
      if((obj_check.pos.x + obj_check.len <= obj[i].pos.x + obj[i].len) && (obj_check.pos.x >= obj[i].pos.x) && obj[i].pos.y == obj_check.pos.y)
      {
        *state = IN_NEST;
        return i;
      }
    }
  }

  if((in_area == true) && (area.is_walkable == false))
    *state = FELL;

  return NEG_VAL;
}

void genera_nemici(object *enemy, object *log, int id_log_frog_on, int id_log_full[])
{
  if(num_enemy < NUM_MIN_LINE)
  {
    int id_rand = NEG_VAL, cont = 0;

    do
    {
      id_rand = rand() % num_log;
      cont++;

      if(cont == MAX_TENT_CYCLE){
        id_rand = NEG_VAL;
        break;
      }
    }
    while((is_in_vect(id_rand, id_log_full, num_enemy) != NEG_VAL) || (id_rand == id_log_frog_on));

    if(id_rand > NEG_VAL)
    {
      enemy[id_rand] = init_object(log[id_rand].pos.x + 1, log[id_rand].pos.y, FROG_LEN, ENEMY_C, id_rand, false, NEG_VAL, N);
      id_log_full[num_enemy] = id_rand;
      num_enemy++;
    }
  }
}