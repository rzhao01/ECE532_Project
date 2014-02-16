#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "common.h"
#include "board.h"
#include "graphics.h"
#include "ai.h"

int main()
{
    SDL_Surface* screen;
    initDisplay (&screen, "connect5");

    srand(time(NULL));

    // outer loop for menu options
    for (;;) {
        BOARD master_board;
        PLAYER Player1, Player2;
        Player1.num = 1;
        Player2.num = 2;

        // set board to empty and draw grid
        init_board(master_board);
        drawBoard(screen, master_board);

        // display main menu and let user choose game mode
        int status = mainMenu(screen, &Player1.life, &Player2.life);
        if (status == 0)
            break;

        int turn = 1;

        // inner loop for game turns
        for (;;) {
            PLAYER Curr_Player = (turn % 2 == 1) ? Player1 : Player2;
            char row, col;
            int status = 0;

            // depending on which turn it is let the appropriate player move
            if (Curr_Player.life == HUMAN)
                status = get_move_player (screen, master_board, Curr_Player.num, &row, &col);
            else
                status = get_move_ai1 (master_board, &row, &col);
   
            if (status == 0)
                break;

            set_square(master_board, row, col, Curr_Player.num);
            drawBoard(screen, master_board);

            if (check_board_full(master_board)) {
                msgFull(screen);
                waitEvent (screen);
                break;
            }
            else if (check_board_win(master_board, Curr_Player.num)) {
                msgWin(screen, Curr_Player.num);
                waitEvent (screen);
                break;
            }

            turn++;
        }
    }

    SDL_Quit();
    return 0;
}
