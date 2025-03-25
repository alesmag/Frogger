#include "utils/utils.h"

extern int *assigned_lanes, num_assigned_lanes, num_car;

void _car(int pipeOut, lane_area road, object *car, int speed)
{
  int i, temp_speed = NEG_VAL, numc = 0, rand_c;

  car->pid = getpid();

  srand(car->pid);
  rand_c = MIN_NUM_CICLI + rand() % (MAX_NUM_CICLI - MIN_NUM_CICLI);

  write(pipeOut, car, sizeof(object));

  while(1)
  {
    if(((car->pos.x == LEN_XT + START_TX) && (car->direction == L)) || ((car->pos.x == START_TX) && (car->direction == R)))
      change_lane(car, road, rand_c, temp_speed, &numc);

    else if(car->direction == L)
      car->pos.x += PASSO;

    else if(car->direction == R)
      car->pos.x -= PASSO;

    write(pipeOut, car, sizeof(object));
    usleep(CAR_TIME_WAIT - (speed * CAR_SPEED_WAIT));

    if(temp_speed > NEG_VAL)
      speed = temp_speed;
  }
}

void create_cars(lane_area *road)
{
  int i, j, x = 0, len_car, cont = 0;
  DIR direction;

  srand(time(NULL));

  direction = rand() % NUM_DIR;
  road->lane_vect = malloc(sizeof(lane) * road->num_lines);

  for(i = road->num_lines - 1; i >= 0; i--)
  {
    road->lane_vect[i].direction = direction;

     if(direction == L)
      direction = R;
    else
      direction = L;
  }

  for(i = 0; i < road->num_lines; i++)
  {
    road->lane_vect[i].num_obj = MIN_CAR + rand() % (MAX_CAR - MIN_CAR);
    road->lane_vect[i].speed = MIN_CAR_SPEED + rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED);
    road->lane_vect[i].obj = malloc(sizeof(object) * road->lane_vect[i].num_obj);

    for(j = 0; j < road->lane_vect[i].num_obj; j++)
    {
      if(rand() % NUM_TYPE_AUTO == CAR)
        len_car = MIN_LEN_CAR + rand() % (MAX_LEN_CAR - MIN_LEN_CAR);
      else
        len_car = MIN_LEN_CAMION + rand() % (MAX_LEN_CAMION - MIN_LEN_CAMION);

      if(j == 0)
      {
        if(direction == L)
          x = road->area.pos.x;
        else
          x = road->area.pos.x + LEN_XT - len_car;
      }

      else
      {
        if(direction == L)
          x = road->lane_vect[i].obj[j - 1].pos.x + road->lane_vect[i].obj[j - 1].len + LEN_BETWEEN_CAR;
        else
          x = road->lane_vect[i].obj[j - 1].pos.x - road->lane_vect[i].obj[j - 1].len - LEN_BETWEEN_CAR - len_car;
      }
      
      road->lane_vect[i].obj[j] = init_object(x, road->area.pos.y + (i * FROG_HEIGHT), len_car, CAR_C, cont, false, 0, road->lane_vect[i].direction);
      cont++;
    }
  }
}

void change_lane(object *car, lane_area road, int rand_c, int temp_speed, int *numc)
{
  int num_rand;

  srand(time(NULL));

  if(*numc == rand_c)
  {
    *numc = 0;

    do
      num_rand = rand() % road.num_lines;
    while(is_in_vect(num_rand,assigned_lanes , num_assigned_lanes) != NEG_VAL);

    assigned_lanes[num_assigned_lanes] = num_rand;
    num_assigned_lanes++;

    temp_speed = road.lane_vect[num_rand].speed;
    car->pos.y = road.area.pos.y + num_rand * FROG_HEIGHT;
    car->direction = road.lane_vect[num_rand].direction;
  
    if(num_assigned_lanes == road.num_lines)
      num_assigned_lanes = 0;
  }

  if(car->direction == L)
    car->pos.x = START_TX - LEN_LINE_WAIT + PASSO;
  else
    car->pos.x = LEN_XT + LEN_LINE_WAIT - PASSO;
  
  *numc += 1;
}

void car_process(int fd[], table *t)
{
  int i, j = 0, count = 0;

  create_cars(&t->road);

  assigned_lanes = malloc(sizeof(int) * t->road.num_lines);
  num_assigned_lanes = 0;

  num_car = 0;

  for(i = 0; i < t->road.num_lines; i++)
    num_car += t->road.lane_vect[i].num_obj;

  pid_t pid_car[num_car];

  i = 0;

  for(count = 0; count < num_car; count++)
  {
    pid_car[count] = fork();

    if(pid_car[count] < 0)
    {
      perror("Car Fork Call");
      _exit(1);
    }

    else if (pid_car[count] == 0)
    {
      close(fd[READING]);
      _car(fd[WRITING], t->road, &t->road.lane_vect[i].obj[j], t->road.lane_vect[i].speed);
    }

    j++;

    if(j == t->road.lane_vect[i].num_obj)
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
    kill(cars[i].pid, SIGKILL);
}