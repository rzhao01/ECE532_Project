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
    assert_dims();

    SDL_Surface* screen;
    initDisplay (&screen, "connect5");

    srand(time(NULL));

    // outer loop for menu options
    for (;;) {
        BOARD master_board;
        AI_PLAYER ai = default_ai();
        PLAYER Player1, Player2;
        Player1.num = P1; Player1.stone = STONE_P1;
        Player2.num = P2; Player2.stone = STONE_P2;

        // set board to empty and draw grid
        init_board (master_board);
        drawBoard(screen, master_board);

        // display main menu and let user choose game mode
        int status = mainMenu(screen, &Player1, &Player2);
        if (status == 0)
            break;

        int turn = 1;

        // inner loop for game turns
        for (;;) {
            PLAYER Curr_P, Opp_P;
            if (turn % 2 == 1) {
                Curr_P = Player1;
                Opp_P = Player2;
            }
            else {
                Curr_P = Player2;
                Opp_P = Player1;   
            }

            COORD move;
            int status = 0;

            // depending on which turn it is let the appropriate player move
            if (Curr_P.type == HUMAN)
                status = get_move_player (screen, master_board, Curr_P, &move);
            else {
                printf ("Working...\n");
                status = get_move_ai2 (ai, master_board, Curr_P, Opp_P, turn, &move);
                printf ("Done.\n");
            }

            if (status == 0)
                break;

            set_square(master_board, move.row, move.col, (int)Curr_P.num+1);
            drawBoard(screen, master_board);

            if (check_board_full(master_board)) {
                msgFull(screen);
                waitEvent (screen);
                break;
            }
            else if (check_board_win(master_board, Curr_P)) {
                msgWin(screen, Curr_P);
                waitEvent (screen);
                break;
            }

            turn++;
        }
    }

    SDL_Quit();
    return 0;
}
