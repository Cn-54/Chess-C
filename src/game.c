#include "game.h"
#include <stdbool.h>
#include <stddef.h>
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
    return COLOUR_EMPTY;
}

static bool rookMovementChecker(Game *game,Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    if (from_x == to_x) { // if it moves on the y

        int direction = (to_y > from_y) ? 1 : -1;

        for (int y = from_y + direction; y != to_y; y += direction) {
            if (game->board[y][from_x] != EMPTY)
                return false;
        }
    }

    if (from_y == to_y) { // if it moves on the x

        int direction = (to_x > from_x) ? 1 : -1;

        for (int x = from_x + direction; x != to_x; x += direction) {
            if (game->board[from_y][x] != EMPTY)
                return false;
        }
    }

    if (move.from == move.to) return false; // cannot move to own square
    return from_x == to_x || from_y == to_y; // if rook stays on the same X or Y its legal
}

static bool knightMovementChecker(Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    return (distX == 2 && distY == 1) || (distX == 1 && distY == 2); // can only move in L shapes
}

static bool bishopMovementChecker(Game *game, Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    // Bishops must move diagonally
    if (distX != distY)
        return false;

    int directionX = (to_x > from_x) ? 1 : -1;
    int directionY = (to_y > from_y) ? 1 : -1;

    // Check for obstructions
    int x = from_x + directionX;
    int y = from_y + directionY;

    while (x != to_x && y != to_y) {
        if (game->board[y][x] != EMPTY)
            return false;

        x += directionX;
        y += directionY;
    }

    return true;
}

static bool queenMovementChecker(Game *game, Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    // Cannot stay on the same square
    if (move.from == move.to)
        return false;

    // Diagonal movement
    if (distX == distY) {

        int directionX = (to_x > from_x) ? 1 : -1;
        int directionY = (to_y > from_y) ? 1 : -1;

        int x = from_x + directionX;
        int y = from_y + directionY;

        while (x != to_x && y != to_y) {
            if (game->board[y][x] != EMPTY)
                return false;

            x += directionX;
            y += directionY;
        }

        return true;
    }

    // Vertical movement
    if (from_x == to_x) {

        int direction = (to_y > from_y) ? 1 : -1;

        for (int y = from_y + direction; y != to_y; y += direction) {
            if (game->board[y][from_x] != EMPTY)
                return false;
        }

        return true;
    }

    // Horizontal movement
    if (from_y == to_y) {

        int direction = (to_x > from_x) ? 1 : -1;

        for (int x = from_x + direction; x != to_x; x += direction) {
            if (game->board[from_y][x] != EMPTY)
                return false;
        }

        return true;
    }

    return false;
    // basically a bishop and rook merged
}

static bool kingMovementChecker(Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    return distX <= 1 && distY <= 1 &&
           (distX != 0 || distY != 0); // can only move 1 square
}


static bool islegalmove(Game *game, Move move){
    if(!(move.from <= 63  && move.to <= 63)) return false;


    Piece piece = game->board[move.from / 8][move.from % 8];
    Piece target = game->board[move.to / 8][move.to % 8];

    if (piece_colour(piece) != game->turn) return false; // cannot move on opponents turn
    if (piece == EMPTY) return false; // moving empty squares not allowed
    if (target != EMPTY && (piece_colour(piece) == piece_colour(target))) return false; // cannot capture own piece

    // peice movement logic
    // TODO: Add logic to determine legal moves here
    switch (piece) {
        case BLACK_PAWN:
        case WHITE_PAWN:
            // TODO: Add pawnMovementChecker
            return false;

        case BLACK_ROOK:
        case WHITE_ROOK:
            return rookMovementChecker(game,move);

        case BLACK_KNIGHT:
        case WHITE_KNIGHT:
            return knightMovementChecker(move);

        case BLACK_BISHOP:
        case WHITE_BISHOP:
            return bishopMovementChecker(game,move);

        case BLACK_QUEEN:
        case WHITE_QUEEN:
            queenMovementChecker(game,move){
            return false;

        case BLACK_KING:
        case WHITE_KING:
            return kingMovementChecker(move);
        }

    return false;
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
