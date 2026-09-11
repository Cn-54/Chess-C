#include "uci.h"
#include "engine.h"

#include <stdio.h>
#include <string.h>

static void handle_uci(void){
    printf("id name Chess-C\n");
    printf("id author Cn-54\n");
    printf("uciok\n");

    fflush(stdout);
}

static void handle_isready(void){
    printf("readyok\n");

    fflush(stdout);
}

static void handle_go(Game *game){
    Move move = Think(game);

    int from_x = move.from % 8;
    int from_y = move.from / 8;

    int to_x = move.to % 8;
    int to_y = move.to / 8;

    // converts game.c format to chess notation
    char from_file = 'a' + from_x;
    char from_rank = '8' - from_y;

    char to_file = 'a' + to_x;
    char to_rank = '8' - to_y;

    printf("bestmove %c%c%c%c\n",
           from_file, from_rank,
           to_file, to_rank);

    fflush(stdout);
}

void UCI_Loop(Game *game){
    char input[4096];

    while (fgets(input, sizeof(input), stdin)) {

        if (strcmp(input, "uci\n") == 0) {
            handle_uci();
        }

        else if (strcmp(input, "isready\n") == 0) {
            handle_isready();
        }

        else if (strcmp(input, "ucinewgame\n") == 0) {
            init_board(game);
        }

        else if (strcmp(input, "position startpos\n") == 0) {
            init_board(game);
        }
        else if (strcmp(input, "go\n") == 0) {
            handle_go(game);
        }

        else if (strcmp(input, "quit\n") == 0) {
            break;
        }
    }
}