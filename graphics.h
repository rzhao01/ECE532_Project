// This file is adapted from code by
// Garret Raziel, Alda (2010)

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "common.h"

void initDisplay (SDL_Surface** screen, char title[]);
int drawBoard(SDL_Surface* screen, BOARD b);

//int clearStatusbar(SDL_Surface*);
int msgStatusbar(SDL_Surface* screen, char* message, int color);
int msgFull(SDL_Surface* screen);
int msgWin(SDL_Surface* screen, PLAYER player);

int mainMenu (SDL_Surface* screen, PLAYER* Player1, PLAYER* Player2);

int waitEvent (SDL_Surface* screen);

int get_move_player (SDL_Surface* screen, BOARD b, PLAYER P, char* row, char* col);

#endif
