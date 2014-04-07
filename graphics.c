// This file is adapted from code by
// Garret Raziel, Alda (2010)

#include "graphics.h"
#include "board.h"
#include <stdio.h>

#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

// open uart tunnel for AI player
int open_uart () {
    int USB;
    struct termios tty;
    memset (&tty, 0, sizeof tty);
    
    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    if ( tcgetattr ( USB, &tty ) != 0 ) {
      fprintf (stderr, "Error %d from tcgetattr: %s\n", errno, strerror(errno));
      return 0;
    }
    /* Save old tty parameters */
    //tty_old = tty;
    
    /* Set Baud Rate */
    cfsetospeed (&tty, (speed_t)B9600);
    cfsetispeed (&tty, (speed_t)B9600);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;

    //tty.c_cflag     &=  ~CRTSCTS;       // no flow control
    tty.c_cc[VMIN]      =   1;                  // read doesn't block
    tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    /* Make raw */
    //cfmakeraw(&tty);

    /* Flush Port, then applies attributes */
    tcflush( USB, TCIFLUSH );

    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
    {
      fprintf (stderr, "Error %d from tcgetattr\n", errno);
      return 0;
    }

    printf ("USB set to %d\n", USB);
    return USB;
}

// initialize screen, set to NULL if graphics flag is not defined
void initDisplay (SDL_Surface** screen, char title[]) {
#ifdef GRAPHICS
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_putenv("SDL_VIDEO_CENTERED=1");
  *screen = SDL_SetVideoMode(BOARD_COLS*SQUARE_SIZE + EXTRA_WIDTH, BOARD_ROWS*SQUARE_SIZE + STATUSBAR_SIZE, 32, SDL_HWSURFACE);
  if(*screen == NULL)
  {
    fprintf(stderr, "SDL: %s\n", SDL_GetError());
    exit(1);
  }
  SDL_WM_SetCaption(title, NULL);
#else
  *screen = NULL;
#endif
}

// draw the grid
int drawGrid(SDL_Surface *screen)
{
  SDL_FillRect(screen, NULL, BLACK);
  for(int i = 0; i < BOARD_COLS+1; ++i) {
    hlineColor(screen, EXTRA_WIDTH/2, BOARD_ROWS*SQUARE_SIZE + EXTRA_WIDTH/2, i*SQUARE_SIZE, GRAY);
  }
  for(int i = 0; i < BOARD_ROWS+1; ++i) {
    vlineColor(screen, i*SQUARE_SIZE + EXTRA_WIDTH/2, 0, BOARD_COLS*SQUARE_SIZE, GRAY);
  }
  SDL_UpdateRect(screen, 0,0,0,0);
  return 0;
}

// draw a single square
int drawSquare(SDL_Surface* screen, char row, char col, char square) {
  if (square == STONE_P1) {
    // draw a cross
    lineColor(screen, col*SQUARE_SIZE+2 + EXTRA_WIDTH/2, row*SQUARE_SIZE+2, 
                      (col+1)*SQUARE_SIZE-2 + EXTRA_WIDTH/2, (row+1)*SQUARE_SIZE-2, RED);
    lineColor(screen, (col+1)*SQUARE_SIZE-2 + EXTRA_WIDTH/2, row*SQUARE_SIZE+2,
                      col*SQUARE_SIZE+2 + EXTRA_WIDTH/2, (row+1)*SQUARE_SIZE-2, RED);
  }
  else if (square == STONE_P2) {
    circleColor(screen, col*SQUARE_SIZE + SQUARE_SIZE/2 + EXTRA_WIDTH/2, row*SQUARE_SIZE + SQUARE_SIZE/2, 
                SQUARE_SIZE/2 - 2, YELLOW);
  }
  return 0; 
}

// draw the board
int drawBoard(SDL_Surface* screen, BOARD b)
{
#ifdef GRAPHICS
  drawGrid (screen);

  for (int row = 0; row < BOARD_ROWS; ++row) {
    for (int col = 0; col < BOARD_COLS; ++col) {
      ELEM elem = get_square (b, row, col);
      if (elem == STONE_P1)
        drawSquare (screen, row, col, STONE_P1);
      else if (elem == STONE_P2)
        drawSquare (screen, row, col, STONE_P2);
    }
  }
  SDL_UpdateRect(screen, 0,0,0,0);
#endif
  return 0;
}

int clearStatusbar(SDL_Surface *screen)
{
#ifdef GRAPHICS
  SDL_Rect statusbar = {.x = 0, .y = BOARD_COLS*SQUARE_SIZE+1, .w = BOARD_ROWS*SQUARE_SIZE+100, .h = STATUSBAR_SIZE};
  SDL_FillRect(screen, &statusbar, BLACK);
  SDL_UpdateRect(screen, 0,0,0,0);
#endif  
  return 0;
}

int msgStatusbar(SDL_Surface *screen, char* message, int color)
{
#ifdef GRAPHICS
  clearStatusbar(screen);
  stringColor(screen, 5, SQUARE_SIZE*BOARD_COLS + 5, message, color);
  SDL_UpdateRect(screen, 0,0,0,0);
#endif
  return 0;
}

int msgFull(SDL_Surface *screen)
{
#ifdef GRAPHICS
  msgStatusbar(screen, "The Game is Drawn. Any Key to Return", RED);
  SDL_Delay(1000);
#endif
  return 0;
}

int msgWin(SDL_Surface *screen, PLAYER player)
{
#ifdef GRAPHICS
  char buffer[200];
  sprintf (buffer, "Player %d [%s] Wins. Any Key to Return", 
                    (int)player.num+1, player.type==HUMAN ? "HUMAN" : "AI");
  msgStatusbar(screen,buffer, RED);
  SDL_Delay(1000);
#endif
  return 0;
}

// selects player type for P
int selectPlayerType(SDL_Surface *screen, PLAYER* P) {
  char buffer[200];
  sprintf (buffer, "Select Player %d: [F1] Human, [F2] AI, [F3] External FPGA", P->num+1);
  msgStatusbar(screen, buffer, WHITE);
  SDL_Event event;

  while(1) {
    if(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        return 0;
      }
      if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
          case SDLK_F1:
            P->type = HUMAN;
            return 1;
            break;
          case SDLK_F2:
            P->type = AI;
            return 1;
            break;
          case SDLK_F3:
            P->type = FPGA;            
            return 1;
          case SDLK_q:
          case SDLK_ESCAPE:
            return 0;
          default:
            break;
        }
      }
    }
  }
  return 0;
}

// displays the menu, waits for user to indicate what game type, assigns Player1 and Player2 accordingly
int mainMenu(SDL_Surface *screen, PLAYER* Player1, PLAYER* Player2)
{
  int res = 1;
  res = res && selectPlayerType (screen, Player1);
  res = res && selectPlayerType (screen, Player2);
  if (Player1->type == FPGA && Player2->type == FPGA) {
    fprintf (stderr, "At most one player can be an FPGA\n");
    return 0;
  }
  clearStatusbar(screen);
  return res;
}

int waitEvent (SDL_Surface* screen) {
  SDL_Event event;
  while (1) {
     if(SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONUP)
        break;
    }
  }
  return 0;
}

int get_move_player (SDL_Surface* screen, BOARD b, PLAYER P, COORD* move) {
  char buffer[200];
  sprintf (buffer, "Waiting for Player %d (Human) to move", (int)P.num+1);
  msgStatusbar(screen, buffer, WHITE);
  SDL_Event event;

  while(1) {
    if(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
        return 0;
      }
      if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        move->col = (event.button.x - EXTRA_WIDTH/2) / SQUARE_SIZE;
        move->row = (event.button.y) / SQUARE_SIZE;
        if(get_square (b, move->row, move->col) == 0) {
          return 1;
        }
      }
    }
    //SDL_Delay(10);
  }
  clearStatusbar(screen);
}
