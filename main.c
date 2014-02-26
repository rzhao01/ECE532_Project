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
        BOARD_SET master_set;
        AI_PLAYER ai = default_ai();
        PLAYER Player1, Player2;
        Player1.num = 0;
        Player2.num = 1;

        // set board to empty and draw grid
        init_board_set(master_set);
        drawBoardSet(screen, master_set);

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

            char row, col;
            int status = 0;

            // depending on which turn it is let the appropriate player move
            if (Curr_P.life == HUMAN)
                status = get_move_player (screen, master_set, Curr_P.num, &row, &col);
            else
                status = get_move_ai1 (ai, master_set, Curr_P.num, Opp_P.num, &row, &col);
   
            if (status == 0)
                break;

            set_square(master_set[Curr_P.num], row, col);
            drawBoardSet(screen, master_set);

            if (check_board_full(master_set)) {
                msgFull(screen);
                waitEvent (screen);
                break;
            }
            else if (check_board_win(master_set, Curr_P, Opp_P)) {
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
