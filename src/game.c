#include "game.h"
#include <stdbool.h>
#include <stdlib.h>


static void init_board(Game *game){
    game->board[0][0] = BLACK_ROOK;
    game->board[0][7] = BLACK_ROOK;

    game->board[0][1] = BLACK_KNIGHT;
    game->board[0][6] = BLACK_KNIGHT;

    game->board[0][2] = BLACK_BISHOP;
    game->board[0][5] = BLACK_BISHOP;

    game->board[0][3] = BLACK_QUEEN;
    game->board[0][4] = BLACK_KING;

    for(size_t i = 0;i < 8; i++){
        game->board[1][i] = BLACK_PAWN ;
    }

    game->board[7][7] = WHITE_ROOK;
    game->board[7][0] = WHITE_ROOK;

    game->board[7][1] = WHITE_KNIGHT;
    game->board[7][6] = WHITE_KNIGHT;

    game->board[7][2] = WHITE_BISHOP;
    game->board[7][5] = WHITE_BISHOP;

    game->board[7][3] = WHITE_QUEEN;
    game->board[7][4] = WHITE_KING;

    for(size_t i = 0;i < 8; i++){
        game->board[6][i] = WHITE_PAWN ;
    }
}

static bool islegalmove(Game *game, Move move);


Game *Create_Game(void){
    Game *game = calloc(1, sizeof(Game));

    if (game == NULL)
        return NULL;

    game->turn = COLOUR_WHITE;
    game->state = GAME_IN_PROGRESS;

    init_board(game);

    return game;
}

void Destroy_Game(Game *game){
    free(game);
}

bool Make_Move(Game *game, Move move)
{
    if (!islegalmove(game, move)) {
        return false;
    }

    Piece piece = game->board[move.from / 8][move.from % 8];

    game->board[move.to / 8][move.to % 8] = piece;
    game->board[move.from / 8][move.from % 8] = EMPTY;

    game->turn = (game->turn == COLOUR_WHITE)? COLOUR_BLACK : COLOUR_WHITE;

    return true;
}
