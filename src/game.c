#include "game.h"
#include <stdbool.h>
#include <stdlib.h>


// HELPERS
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

static Colour piece_colour(Piece piece){
    if(piece == EMPTY) return COLOUR_EMPTY;
    if (piece >= WHITE_PAWN && piece <= WHITE_KING) return COLOUR_WHITE;
    if (piece >= BLACK_PAWN && piece <= BLACK_KING) return COLOUR_BLACK;
    return COLOUR_EMPTY
}

static bool islegalmove(Game *game, Move move){
    if(!(move.from <= 63  && move.to <= 63)) return false;

    // TODO:
    // add logic to determine legal moves here

    Piece piece = game->board[move.from / 8][move.from % 8];
    Piece target = game->board[move.to / 8][move.to % 8];

    if (piece_colour(piece) != game->turn) return false; // cannot move on opponents turn
    if (piece == EMPTY) return false; // moving empty squares not allowed
    if (target != EMPTY && (piece_colour(piece) == piece_colour(target))) return false; // cannot capture own piece
    
    return true;
}

// HELPERS END

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

    Piece piece = game->board[move.from / 8][move.from % 8]; // from and too stored as an 8 bit num 0-63 so /8 and %8 are for the x and y

    game->board[move.to / 8][move.to % 8] = piece;
    game->board[move.from / 8][move.from % 8] = EMPTY;

    game->turn = (game->turn == COLOUR_WHITE)? COLOUR_BLACK : COLOUR_WHITE;

    return true;
}
