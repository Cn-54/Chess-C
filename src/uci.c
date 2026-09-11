#include "uci.h"
#include "engine.h"

#include <stdio.h>
#include <string.h>

static int parse_square(const char *square){
    int file = square[0] - 'a';
    int rank = '8' - square[1];

    return rank * 8 + file;
}

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

static void handle_position(Game *game, char *input){
    char *token = strtok(input, " \n");

    token = strtok(NULL, " \n");

    if (token == NULL)
        return;

    if (strcmp(token, "startpos") == 0) {

        token = strtok(NULL, " \n");

        if (token == NULL)
            return;

        if (strcmp(token, "moves") != 0)
            return;

        while ((token = strtok(NULL, " \n")) != NULL) {

            int from = parse_square(token);
            int to = parse_square(token + 2);

            Move move = {
                .from = from,
                .to = to
            };

            if (!Make_Move(game, move))
                return;
        }
    }
}

void UCI_Loop(Game *game){
    char input[4096];

    while (fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "RECEIVED: %s", input);

        if (strcmp(input, "uci\n") == 0) {
            handle_uci();
        }

        else if (strcmp(input, "isready\n") == 0) {
            handle_isready();
        }

        else if (strcmp(input, "ucinewgame\n") == 0) {
            Reset_game(game);
        }

        else if (strncmp(input, "position", 8) == 0) {
            Reset_game(game);
            handle_position(game, input);
        }
        else if (strncmp(input, "go", 2) == 0) {
            handle_go(game);
        }

        else if (strcmp(input, "quit\n") == 0) {
            break;
        }
    }
}