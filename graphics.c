// This file is adapted from code by
// Garret Raziel, Alda (2010)

#include "graphics.h"
#include "board.h"

// initialize screen, set to NULL if graphics flag is not defined
void initDisplay (SDL_Surface** screen, char title[]) {
#ifdef GRAPHICS
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "SDL: %s\n", SDL_GetError());
    exit(1);
  }

  //SDL_putenv("SDL_VIDEO_CENTERED=1"); //FIXME: okno doprostred, ale vyvola warning pri kompilaci
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
  if (square == P1) {
    // draw a cross
    lineColor(screen, col*SQUARE_SIZE+2 + EXTRA_WIDTH/2, row*SQUARE_SIZE+2, 
                      (col+1)*SQUARE_SIZE-2 + EXTRA_WIDTH/2, (row+1)*SQUARE_SIZE-2, RED);
    lineColor(screen, (col+1)*SQUARE_SIZE-2 + EXTRA_WIDTH/2, row*SQUARE_SIZE+2,
                      col*SQUARE_SIZE+2 + EXTRA_WIDTH/2, (row+1)*SQUARE_SIZE-2, RED);
  }
  else if (square == P2) {
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
      char square = get_square (b, row, col);
      drawSquare (screen, row, col, square);
    }
  }
  SDL_UpdateRect(screen, 0,0,0,0);
#endif
  return 0;
}

int clearStatusbar(SDL_Surface *screen)
{
#ifdef GRAPHICS
  SDL_Rect statusbar = {.x = 0, .y = BOARD_COLS*SQUARE_SIZE+1, .w = BOARD_ROWS*SQUARE_SIZE, .h = STATUSBAR_SIZE};
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

int msgWin(SDL_Surface *screen, int player)
{
#ifdef GRAPHICS
  char buffer[200];
  sprintf (buffer, "Player %d Wins. Any Key to Return", player);
  msgStatusbar(screen,buffer, RED);
  SDL_Delay(1000);
#endif
  return 0;
}

// displays the menu, waits for user to indicate what game type, assigns Player1 and Player2 accordingly
int mainMenu(SDL_Surface *screen, int* Player1, int* Player2)
{
  msgStatusbar(screen, "[F1] Human vs Human, [F2] Human vs AI, [F3] AI vs Human, [F4] AI vs AI", WHITE);
  SDL_Event event;

  while(1) {
    if(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        return 0;
      }
      if(event.type == SDL_KEYDOWN) {
        switch(event.key.keysym.sym) {
          case SDLK_F1:
            *Player1 = HUMAN;
            *Player2 = HUMAN;
            return 1;
          case SDLK_F2:
            *Player1 = HUMAN;
            *Player2 = AI;
            return 1;
          case SDLK_F3:
            *Player1 = AI;
            *Player2 = HUMAN;
            
            return 1;
          case SDLK_F4:
            *Player1 = AI;
            *Player2 = AI;
            return 1;
          case SDLK_q:
          case SDLK_ESCAPE:
            return 0;
          default:
            break;
        }
      }
    }
    //SDL_Delay(10);
  }
  clearStatusbar(screen);
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

int get_move_player (SDL_Surface* screen, BOARD b, int Player, char* row, char* col) {
  char buffer[200];
  sprintf (buffer, "Waiting for Player %d (Human) to move", Player);
  msgStatusbar(screen, buffer, WHITE);
  SDL_Event event;

  while(1) {
    if(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q)) {
        return 0;
      }
      if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        *col = (event.button.x - EXTRA_WIDTH/2) / SQUARE_SIZE;
        *row = (event.button.y) / SQUARE_SIZE;
        if(get_square(b, *row, *col) == EMPTY) {
          return 1;
        }
      }
    }
    //SDL_Delay(10);
  }
  clearStatusbar(screen);
}
