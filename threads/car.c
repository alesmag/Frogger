#include "utils/utils.h"

extern table t;
extern int *assigned_lanes, num_assigned_lanes, num_car;

void* _car(void* arg)
{
  int i, temp_speed = NEG_VAL, numc = 0, rand_c, line = 0;
  int id = (int) arg;
  object car = which_obj(t.road, id, &line);

  car.th_id = pthread_self();

  srand(car.th_id);
  rand_c = MIN_NUM_CICLI + rand() % (MAX_NUM_CICLI - MIN_NUM_CICLI);

  push_obj(car);

  while(1)
  {
    if(((car.pos.x == LEN_XT + START_TX) && (car.direction == L)) || ((car.pos.x == START_TX) && (car.direction == R)))
      change_lane(&car, rand_c, temp_speed, &numc);

    else if(car.direction == L)
      car.pos.x += PASSO;

    else if(car.direction == R)
      car.pos.x -= PASSO;

    push_obj(car);
    usleep(CAR_TIME_WAIT - (t.road.lane_vect[line].speed * CAR_SPEED_WAIT));

    if(temp_speed > NEG_VAL)
      t.road.lane_vect[line].speed = temp_speed;
  }
}

void create_cars()
{
  int i, j, x = 0, len_car, cont = 0;
  DIR direction;

  srand(time(NULL));

  direction = rand() % NUM_DIR;
  t.road.lane_vect = malloc(sizeof(lane) * t.road.num_lines);

  for(i = t.road.num_lines - 1; i >= 0; i--)
  {
    t.road.lane_vect[i].direction = direction;

     if(direction == L)
      direction = R;
    else
      direction = L;
  }

  for(i = 0; i < t.road.num_lines; i++)
  {
    t.road.lane_vect[i].num_obj = MIN_CAR + rand() % (MAX_CAR - MIN_CAR);
    t.road.lane_vect[i].speed = MIN_CAR_SPEED + rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED);
    t.road.lane_vect[i].obj = malloc(sizeof(object) * t.road.lane_vect[i].num_obj);

    for(j = 0; j < t.road.lane_vect[i].num_obj; j++)
    {
      if(rand() % NUM_TYPE_AUTO == CAR)
        len_car = MIN_LEN_CAR + rand() % (MAX_LEN_CAR - MIN_LEN_CAR);
      else
        len_car = MIN_LEN_CAMION + rand() % (MAX_LEN_CAMION - MIN_LEN_CAMION);

      if(j == 0)
      {
        if(direction == L)
          x = t.road.area.pos.x;
        else
          x = t.road.area.pos.x + LEN_XT - len_car;
      }

      else
      {
        if(direction == L)
          x = t.road.lane_vect[i].obj[j - 1].pos.x + t.road.lane_vect[i].obj[j - 1].len + LEN_BETWEEN_CAR;
        else
          x = t.road.lane_vect[i].obj[j - 1].pos.x - t.road.lane_vect[i].obj[j - 1].len - LEN_BETWEEN_CAR - len_car;
      }
      
      t.road.lane_vect[i].obj[j] = init_object(x, t.road.area.pos.y + (i * FROG_HEIGHT), len_car, CAR_C, cont, false, NEG_VAL, t.road.lane_vect[i].direction);
      cont++;
    }
  }
}

void change_lane(object *car, int rand_c, int temp_speed, int *numc)
{
  int num_rand;

  srand(time(NULL));

  if(*numc == rand_c)
  {
    *numc = 0;

    do
      num_rand = rand() % t.road.num_lines;
    while(is_in_vect(num_rand, assigned_lanes, num_assigned_lanes) != NEG_VAL);

    assigned_lanes[num_assigned_lanes] = num_rand;
    num_assigned_lanes++;

    temp_speed = t.road.lane_vect[num_rand].speed;
    car->pos.y = t.road.area.pos.y + num_rand * FROG_HEIGHT;
    car->direction = t.road.lane_vect[num_rand].direction;
  
    if(num_assigned_lanes == t.road.num_lines)
      num_assigned_lanes = 0;
  }

  if(car->direction == L)
    car->pos.x = START_TX - LEN_LINE_WAIT + PASSO;
  else
    car->pos.x = LEN_XT + LEN_LINE_WAIT - PASSO;
  
  *numc += 1;
}

void car_process()
{
  int i, j = 0, count = 0;

  create_cars();

  assigned_lanes = malloc(sizeof(int) * t.road.num_lines);
  num_assigned_lanes = 0;

  num_car = 0;

  for(i = 0; i < t.road.num_lines; i++)
    num_car += t.road.lane_vect[i].num_obj;

  pthread_t thread_car[num_car];

  i = 0;

  for(count = 0; count < num_car; count++)
  {
    pthread_create(&thread_car[count], NULL, _car, count);

    j++;

    if(j == t.road.lane_vect[i].num_obj)
    {
      i++;
      j = 0; 
    }

  }

  free(assigned_lanes);
}

void close_cars(object cars[])
{
  for(int i = 0; i < num_car; i++)
    pthread_cancel(cars[i].th_id);
}