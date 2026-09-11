#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_MOVE_HISTORY 1024

typedef enum {
    EMPTY,
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING
} Piece;

typedef enum {
    COLOUR_EMPTY,
    COLOUR_WHITE,
    COLOUR_BLACK
} Colour;

typedef enum {
    GAME_IN_PROGRESS,
    GAME_WHITE_WINS,
    GAME_BLACK_WINS,
    GAME_DRAW
} GameState;

typedef struct {
    uint8_t from;
    uint8_t to;

} Move;

typedef struct {
    Move move;
    Piece captured_piece;
    Colour previous_turn;
} MoveHistory;

typedef struct {
    Move moves[256];
    size_t count;
} MoveList;

typedef struct {
    Piece board[8][8];
    Colour turn;
    GameState state;

    MoveHistory history[MAX_MOVE_HISTORY];
    int move_num;

} Game;


Game *Create_Game(void);
void Destroy_Game(Game *game);
void init_board(Game *game);
void Reset_game(Game *game);
bool Make_Move(Game *game, Move move);
void Undo_Move(Game *game);
MoveList GenerateMoves(Game *game);


#endif