/* LIBRARIES */

// Librerie standard
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Librerie per la gestione dei processi e delle pipes
#include <signal.h>
#include <unistd.h>         

// Librerie per le chiamate a sistema
#include <sys/types.h>  
#include <sys/wait.h>       

#include <sys/time.h>       // Libreria per tenere traccia del tempo
#include <time.h>           // Libreria per la generazione casuale
#include <ncurses.h>        // Libreria ncurses

/* DEFINES */

// Generic
#define NEG_VAL -1
#define PASSO 1 
#define NUM_DIR 2
#define MAX_TIMER 40
#define NUM_VITE 5
#define ENTER 10
#define PAUSE_CREATE_ENEMY 8
#define FD_SIZE 2
#define DEATH_WAIT 500000
#define MAX_TENT_CYCLE 5
#define TIMER_LEN 6
#define TIMER_C 't'
#define ENEMY_C 'e'

// Score
#define STEP_SCORE 100
#define NEST_SCORE 500
#define TIME_SCORE 200
#define ENEMY_SCORE 250

// Screen
#define START_TX 0
#define START_TY 2
#define LEN_XT 62
#define MIN_DIM_XW 56
#define MIN_X 120
#define MIN_Y 30

// Car
#define MAX_CAR_SPEED 4
#define MIN_CAR_SPEED 1
#define MIN_CAR 2
#define MAX_CAR 4
#define MIN_LEN_CAR 4 
#define MAX_LEN_CAR 6
#define MIN_LEN_CAMION 7 
#define MAX_LEN_CAMION 10
#define LEN_BETWEEN_CAR MIN_LEN_CAMION 
#define LEN_LINE_WAIT MAX_LEN_CAMION * 3
#define NUM_TYPE_AUTO 2 
#define MAX_NUM_CICLI 6
#define MIN_NUM_CICLI 3
#define CAR_TIME_WAIT 120000
#define CAR_SPEED_WAIT 3000
#define CAR_C 'c'

// Frog
#define FROG_HEIGHT 2
#define FROG_LEN 4
#define STEP 2 
#define UP 65
#define DOWN 66
#define LEFT 68
#define RIGHT 67
#define SPACE 32
#define FROG_WAIT 100
#define FROG_C 'f'

// Log
#define NUM_LOG_LINE 1
#define MAX_LOG_SPEED 3
#define MIN_LOG_SPEED 1
#define LOG_WAIT 95000
#define LOG_C 'l'

// Inits
#define NUM_NEST 5
#define NUM_MAX_LINE 5
#define NUM_MIN_LINE 3

// Projectiles
#define BULLET_WAIT 60000
#define MAX_BULLET 200
#define BULLET_C 'b'
#define LASER_C 's'

// Menu
#define NUM_VOCI_MENU 3
#define NUM_VOCI_END 2
#define NUM_STRING_INFO 16


/* TYPEDEFS */


// Generic

// Utilizzato per la scrittura o la lettura dalla pipe
typedef enum {READING, WRITING} descriptorType;

// Utilizzato per la gestione dei colori
typedef enum {BLACK_BLACK, BLACK_GREEN, WHITE_BLUE, WHITE_BLACK, BLACK_WHITE, BLACK_MAG, BLACK_CYAN, BLACK_YELLOW} Colours;

// Utilizzato per la selezione della difficolta'
typedef enum {NORMAL, HARD} Difficulty;


// Frog

// Utilizzato per la gestione degli stati della rana
typedef enum {HIT, FELL, ON_OBJ, END_LIVES, END_TIME, KILLED, RUNNING, IN_NEST, NEST_FULL} FROG_STATE;


// Car

// Utilizzato per gestire il tipo di auto generata
typedef enum {CAR, CAMION} TYPE_AUTO;


// Car, Log

// Utilizzato per la gestione delle direzioni (L: partenza da sinistra, R: partenza da destra, N: null)
typedef enum {R, L, N} DIR;


// Menu

// Utilizzato per selezionare le opzioni nel menu
typedef enum {OPT1, OPT2, OPT3} Options;


typedef struct
{
  int x;
  int y;
} coord;

typedef struct 
{
  coord pos;
  int height;
  bool is_walkable;
  char texture;
} game_area;

typedef struct
{
  coord pos;
  int len;
  char c;
  int id;
  bool is_full;
  pid_t pid;
  DIR direction;
} object;

typedef struct
{
  game_area grass;
  int num_nest;
  object nest[NUM_NEST];
} high_grass;

typedef struct
{
  object *obj;
  int num_obj;
  int speed;
  DIR direction;
} lane;

typedef struct 
{
  game_area area;
  lane *lane_vect;   
  int num_lines;
} lane_area;

typedef struct 
{
  high_grass hg;
  lane_area river, road;
  game_area grass, start;
} table;

/* PROTOTYPES */

/* Utils */

// Controlla la dimensione della finestra
void consoleSize();

// Gestione del processo timer
void timer_process(int fd[]);

// Chiusura del processo timer
void close_timer(object timer);

// Chiude tutti i processi creati
void close_all(object timer, object frog, object cars[], object logs[], object bullets[], object lasers[]);

// Libera la memoria utilizzata dalle corsie
void free_lanes(table *t);

// Compatta l'array passato e ne riduce la dimensione
void compact_array(int *vect, int *n);

// Controlla se tutte le tane sono piene
bool is_nests_full(table t);

// Controlla se un dato valore intero si trova in un dato vettore
int is_in_vect(int id, int *vect, int n);


/* Car */

// Funzione richiamata per il processo macchina
void _car(int pipeOut, lane_area road, object *car, int speed);

// Crea le auto che saranno presenti sulla strada
void create_cars(lane_area *road);

// Gestisce il cambio di corsia
void change_lane(object *car, lane_area road, int rand_c, int temp_speed, int *numc);

// Gestisce il processo di una macchina
void car_process(int fd[], table *t);

// Chiude il processo di ogni macchina
void close_cars(object cars[]);


/* Frog */

// Funzione richiamata per il processo rana
void _frog(int pipeOut, table t);

// Gestisce il processo della rana
void frog_process(int fd[], table t);

// Chiude il processo rana
void close_frog(object frog);


/* Game Area */

// Gestisce l'insieme dei componenti del gioco
void area_gioco(WINDOW *win, int *pipe, table *t);

// Controllo per le collisioni con un tronco
int log_collision(game_area area ,object * obj , int num, object obj_check, FROG_STATE *state, int id_log_full[]);

// Controllo per le collisioni con una macchina
int car_collision(game_area area, object *obj, int num, object obj_check, FROG_STATE *state);

// Controllo per la collisione con una tana
int nest_collision(game_area area, object *obj, int num, object obj_check, FROG_STATE *state);

// Genera casualmente i nemici sui tronchi
void genera_nemici(object *enemy, object *log, int id_log_frog_on, int id_log_full[]);


/* Inits */

// Inizializza una game_area
game_area init(int x, int y, int h, bool is_walkable, char t);

// Inizializza la parte alta (in cui si trovano le tane)
void init_high_grass(high_grass *hg);

// Crea le tane nella parte alta
void create_nest(high_grass *hg);

// Inizializza il fiume
void init_river(lane_area *river, high_grass hg);

// Inizializza la strada
void init_road(lane_area *road, game_area grass);

// Inizializza tutte le componenti del tabellone
void init_table(table *t);

// Inizializza un oggetto
object init_object(int x, int y, int len, char c, int id, bool is_full, pid_t pid, DIR direction);

// Assegna ad un oggetto dei valori nulli o negativi
object reset_object();

// Inizializza i colori per ncurses
void init_colors();

// Inizializza la pipe per i processi
void init_pipe(int fd[]);


/* Log */

// Funzione richiamata per i processi dei tronchi
void _log(int pipeOut, object *log, DIR direction, int speed, coord pos, int line, int id);

// Crea i tronchi sul fiume
void create_logs(lane_area *river);

// Gestisce il processo del tronco
void log_process(int fd[], table *t);

// Chiude i processi dei tronchi
void close_logs(object logs[]);


/* Menus */

// Gestisce il menu principale
bool menu();

// Gestisce la finestra finale
bool end_box(int string_id);

// Gestisce la finestra per le informazioni del gioco contenute nel menu
void info_msg();

// Gestisce la finestra per la selezione della difficolta'
bool difficulty();


/* Projectiles */

// Funzione richiamata per i processi proiettile della rana
void _bullet(int pipeOut, int y, int x);

// Gestisce la generazione dei proiettili nemici
void genera_bullet(object *enemy, int pipeOut, int id_log_full[]);

// Funzione richiamata per i processi proiettile dei nemici
void _laser(int pipeOut, int y, int x);

// Controllo delle collisioni con un proiettile nemico e la rana
int laser_frog_collision(object frog, object *laser);

// Controllo delle collisioni con un proiettile nemico e un proiettile rana
int laser_bullet_collision(object bullet, object *laser);

// Controllo delle collisioni con un proiettile rana e un nemico
int bullet_enemy_collision(object *enemy, object bullet, int id_log_full[]);

// Chiude i processi dei proiettili rana
void close_bullets(object *bullets);

// Chiude i processi dei proiettili nemici
void close_lasers(object *lasers);


/* Table */

// Disegna la parte alta del tabellone
void draw_high_grass(WINDOW *win, high_grass hg);

// Disegna una specifica area passata alla funzione
void draw_area(WINDOW *win, game_area area, int colour_id);

// Disegna una specifica zona passata alla funzione
void draw_zone(WINDOW *win, coord pos, int len, int height, char c, int colour_id);

// Disegna uno specifico oggetto passato alla funzione
void draw_object(WINDOW *win, object obj);

// Disegna tutti i componenti del tabellone
void draw_table(WINDOW *bg, table t);

// Ridisegna tutti i componenti del tabellone
void reset_table(WINDOW *win, object obj, table t, int height);

// Disegna la rana all'interno di una nest in cui e' entrata
void draw_frog_in_nest(WINDOW *win, table t);

// Disegna le statistiche nella parte alta dello schermo
void draw_stats(WINDOW *win);