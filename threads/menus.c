#include "utils/utils.h"

extern int max_y, max_x, score, diff;

bool menu()
{
  bool cond = false;
  int scelta, highlight = 0;
  char *voci[NUM_VOCI_MENU] = {"NEW GAME", "INFO", "EXIT"};
  WINDOW *scritta = newwin(max_y / 4 + 3, max_x, max_y / 6, max_x / 4);

  system("aplay AUDIO/menumusic.wav 2> AUDIO/dump.txt &");

  mvwprintw(scritta, 0, 0, 
                          "######   ######    ######   ######   ######   ######   ######\n"
                          "##       ##  ##    ##  ##   ##       ##       ##       ##  ##\n"
                          "##       ##  ##    ##  ##   ##       ##       ##       ##  ##\n"
                          "######   ##  ##    ##  ##   ##       ##       ######   ##  ##\n"
                          "##       #####     ##  ##   ##  ##   ##  ##   ##       #####\n"
                          "##       ##  ##    ##  ##   ##  ##   ##  ##   ##       ##  ##\n"
                          "##       ##   ##   ######   ######   ######   ######   ##   ##\n\n"
                          "   (By: Anna Chiara Mameli & Alessio Giuseppe Muggittu)");
  wrefresh(scritta);

  WINDOW *menuwin = newwin(max_y / 4, max_x / 2 + 2, max_y / 2, max_x / 4);
  box(menuwin, 0, 0);
  wrefresh(menuwin);

  // utilizza le frecce per selezionare
  keypad(menuwin, true);

  while(true)
  {
    for(int i = 0; i < NUM_VOCI_MENU; i++)
    {
      // Se il contatore e' uguale alla selezione utente allora il testo selezionato lampeggia
      if(i == highlight)
      {
        wattron(menuwin, A_REVERSE);
        wattron(menuwin, A_BLINK);
      }
      
      mvwaddstr(menuwin, 1 + i * 2, 1, voci[i]);

      wattroff(menuwin, A_REVERSE);
      wattroff(menuwin, A_BLINK);
    }

    scelta = wgetch(menuwin);

    // highlight seleziona una delle tre opzioni del menu
    // Con la freccia su si sale, mentre con quella giu si scende
    switch(scelta)
    {
      case KEY_UP:
        highlight--;
        
        if(highlight < OPT1)
          highlight = OPT3;
      break;
      
      case KEY_DOWN:
        highlight++;

        if(highlight > OPT3)
          highlight = OPT1;
      break;
      
      default:
        break;
    }

    // Quando viene premuto enter si conferma l'opzione selezionata
    if(scelta == ENTER)
    {
      switch(highlight)
      {
        // Porta alla selezione della difficolta' altrimenti ridisegna il menj
        case OPT1:
          cond = difficulty();

          if(cond == false)
          {
            redrawwin(menuwin);
            wrefresh(menuwin);
          }

          else
            refresh();
        break;

        // Stampa un messaggio contenente informazioni di gioco
        case OPT2:
          info_msg();
          redrawwin(scritta);
          redrawwin(menuwin);
          wrefresh(scritta);
          wrefresh(menuwin);
          refresh();
        break;

        // Chiude le finestre ed esce
        case OPT3:
          delwin(scritta);
          delwin(menuwin);
          refresh();
          return false;
        break;

        default:
          break;
      }

      if(highlight != 1 && cond != false)
        break;
    }
  }

  delwin(scritta);
  delwin(menuwin);
  clear();
  refresh();

  return true;
}

bool end_box(int string_id)
{
  int scelta, highlight = 0;
  char *string[NUM_VOCI_END] = {"GAME OVER", "YOU WIN"};
  WINDOW *end = newwin(max_y / 3, max_x / 3, max_y / 3, max_x / 2 - 20);
  box(end, 0, 0);
  wrefresh(end);

  system("killall aplay");

  if(strcmp(string[string_id], "GAME OVER") == 0)
    system("aplay AUDIO/gameover.wav 2> AUDIO/dump.txt &");
  else
    system("aplay AUDIO/victory.wav 2> AUDIO/dump.txt &");

  keypad(end, true);

  mvwaddstr(end, 1, (max_x / 3) / 3 + 2, string[string_id]);
  mvwprintw(end, 3, (max_x / 3) / 3 - 2, "FINAL SCORE: %d", score);
  mvwprintw(end, 5, (max_x / 3) / 3 + 1, "PLAY AGAIN?");

  while(true)
  {
    for(int i = 0; i < NUM_VOCI_END; i++)
    {
      if(i == highlight)
      {
        wattron(end, A_REVERSE);
        wattron(end, A_BLINK);
      }
      
      if(i == OPT1)
        mvwprintw(end, 7, (max_x / 3) / 3, "YES");
      
      if(i == OPT2)
        mvwprintw(end, 7, (max_x / 3) / 2 + 2, "NO");

      wattroff(end, A_REVERSE);
      wattroff(end, A_BLINK);
    }

    scelta = wgetch(end);

    switch(scelta)
    {
      case KEY_RIGHT:
        highlight++;

        if(highlight > OPT2)
          highlight = OPT2;
      break;

      case KEY_LEFT:
        highlight--;

        if(highlight < OPT1)
          highlight = OPT1;
      break;

      default:
        break;
    }

    if(scelta == ENTER)
    {
      switch(highlight)
      {
        // Riproduce la musica del menu e ricomincia
        case OPT1:
          system("killall aplay");
          system("aplay AUDIO/menumusic.wav 2> AUDIO/dump.txt &");
          delwin(end);
          clear();
          refresh();
          return false;
        break;

        // Chiude tutte le musiche e termina
        case OPT2:
          system("killall aplay");
          delwin(end);
          clear();
          refresh();
          return true;
        break;

        default:
          break;
      }

      break;
    }
  }
}

void info_msg()
{
  char *str[NUM_STRING_INFO] = {
                                "Move with ARROW KEYS. You need to bring the frog in each", 
                                "nest to win. You also get points for: going up, killing an", 
                                "enemy, going inside a nest and if you have time left for",
                                "each round. Going down takes points from you.",
                                "",
                                "You can lose lives by: hitting any car, falling in",
                                "water, hitting any enemy, getting hit by any enemy",
                                "bullet and failing to get inside a nest.",
                                "",
                                "You can kill enemies by shooting a bullet with SPACE.",
                                "Enemies will get back after some time.",
                                "",
                                "STEP UP = +100p | STEP DOWN = -100p | NEST = +500p",
                                "TIME LEFT = +200p x SEC | ENEMY KILLED = +250p",
                                "",
                                "Press ENTER to go back"
                              };

  WINDOW *info = newwin(max_y / 2 + 2, max_x / 2 + 2, max_y / 6, max_x / 4);
  box(info, 0, 0);
  wrefresh(info);

  keypad(info, true);

  // Finche' non viene premuto enter continuera' a visualizzare il messaggio
  do
  {
    for(int i = 0; i < NUM_STRING_INFO; i++)
      mvwaddstr(info, i + 1, 1, str[i]);
  } while(wgetch(info) != ENTER);

  keypad(info, false);

  delwin(info);
  refresh();
}

bool difficulty()
{
  int highlight = 0, scelta;
  char *voci[NUM_VOCI_MENU] = {"NORMAL", "HARD", "GO BACK"};
  WINDOW *diffwin = newwin(max_y / 4, max_x / 2 + 2, max_y / 2, max_x / 4);
  box(diffwin, 0, 0);
  wrefresh(diffwin);

  keypad(diffwin, true);

  while(true)
  {
    for(int i = 0; i < NUM_VOCI_MENU; i++)
    {
      if(i == highlight)
      {
        wattron(diffwin, A_REVERSE);
        wattron(diffwin, A_BLINK);
      }
      
      mvwaddstr(diffwin, 1 + i * 2, 1, voci[i]);

      wattroff(diffwin, A_REVERSE);
      wattroff(diffwin, A_BLINK);
    }

    scelta = wgetch(diffwin);

    switch(scelta)
    {
      case KEY_UP:
        highlight--;
        
        if(highlight < OPT1)
          highlight = OPT3;
      break;
      
      case KEY_DOWN:
        highlight++;

        if(highlight > OPT3)
          highlight = OPT1;
      break;
      
      default:
        break;
    }

    if(scelta == ENTER)
    {
      switch(highlight)
      {
        // Seleziona la difficolta' normale e avvia il gioco
        case OPT1:
          diff = NORMAL;
          delwin(diffwin);
          refresh();
          return true;
        break;

        // Seleziona la difficolta' difficile e avvia il gioco
        case OPT2:
          diff = HARD;
          delwin(diffwin);
          refresh();
          return true;
        break;

        // Chiude la selezione della difficolta' e torna al menu
        case OPT3:
          delwin(diffwin);
          refresh();
          return false;
        break;

        default:
          break;
      }

      break;
    }
  }
}