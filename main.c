#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <assert.h>
#include <unistd.h>     // UNIX standard function definitions
#include <errno.h>      // Error number definitions

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

        // hack
        Player2.type = FPGA;

        // open uart tunnel for AI players
        int USB = open_uart ();
        
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
            else if (Curr_P.type == AI) {
                // get the AI move
                printf ("Working...\n");
                status = get_move_ai2 (master_board, Curr_P, Opp_P, turn, &move);
                printf ("Done. AI move (%d,%d)\n", move.row, move.col);
            }
            else {
                // player is an FPGA, read the move from UART
                assert (USB != 0);

                int n = 0;
                char buf;
                char str_buf[5];
                char response[50];
                do {
                    n = read( USB, &buf, 1 );
                    str_buf[0] = buf;
                    str_buf[1] = '\0';
                    strcat (response, str_buf);
                }
                while( buf != '\r' && n > 0);

                if (n < 0) {
                    fprintf (stderr, "Error reading from FPGA (USB=%d): %s\n", USB, strerror(errno));
                }
                else if (n == 0) {
                    fprintf (stderr, "Read nothing from FPGA!\n");
                }
                else {
                    printf ("Read string from FPGA: %s\n", response);
                }
            }

            if (status == 0) {
                fprintf (stderr, "No move generated!\n");
                break;
            }

            set_square(master_board, move.row, move.col, (int)Curr_P.num+1);
            drawBoard(screen, master_board);

            // if opponent is fpga, write move to UART
            if (Opp_P.type == FPGA) {
                assert (USB != 0);
                char cmd[50];
                sprintf (cmd, "%c,%c\r\n", 'A'+move.col, 'A'+move.row);//+move.col, 'A'+move.row);
                printf ("Wrote string to FPGA (USB = %d): %s", USB, cmd);
                int n_written = 0;

                do {
                    n_written += write( USB, &cmd[n_written], 1 );
                }
                while (cmd[n_written-1] != '\r' && n_written > 0);
            }
                
            if (check_board_full(master_board)) {
                msgFull(screen);
                waitEvent (screen);
                break;
            }
            else if (check_board_win(master_board, Curr_P, Opp_P)) {
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
