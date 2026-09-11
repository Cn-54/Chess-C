#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


// HELPERS
void init_board(Game *game){
    // Clear the entire board
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            game->board[y][x] = EMPTY;
        }
    }

    // Black pieces
    game->board[0][0] = BLACK_ROOK;
    game->board[0][1] = BLACK_KNIGHT;
    game->board[0][2] = BLACK_BISHOP;
    game->board[0][3] = BLACK_QUEEN;
    game->board[0][4] = BLACK_KING;
    game->board[0][5] = BLACK_BISHOP;
    game->board[0][6] = BLACK_KNIGHT;
    game->board[0][7] = BLACK_ROOK;

    for (int x = 0; x < 8; x++) {
        game->board[1][x] = BLACK_PAWN;
    }

    // White pieces
    game->board[7][0] = WHITE_ROOK;
    game->board[7][1] = WHITE_KNIGHT;
    game->board[7][2] = WHITE_BISHOP;
    game->board[7][3] = WHITE_QUEEN;
    game->board[7][4] = WHITE_KING;
    game->board[7][5] = WHITE_BISHOP;
    game->board[7][6] = WHITE_KNIGHT;
    game->board[7][7] = WHITE_ROOK;

    for (int x = 0; x < 8; x++) {
        game->board[6][x] = WHITE_PAWN;
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

    if (move.from == move.to)return false;

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

static bool pawnMovementChecker(Game *game, Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;

    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int direction;
    int starting_y;

    Piece target = game->board[to_y][to_x];

    if (game->turn == COLOUR_WHITE) {
        direction = -1;
        starting_y = 6;
    } else {
        direction = 1;
        starting_y = 1;
    }

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    // Move forward one square
    if (distX == 0 && to_y == from_y + direction) return target == EMPTY;

    // Move forward two squares from starting position
    if (distX == 0 && from_y == starting_y && to_y == from_y + (2 * direction)) {

        // Both squares must be empty
        int middle_y = from_y + direction;

        return game->board[middle_y][from_x] == EMPTY && target == EMPTY;
    }

    // Capturing
    if (distX == 1 && to_y == from_y + direction) return target != EMPTY && piece_colour(target) != game->turn;

    return false;
}



static bool islegalmove(Game *game, Move move){
    if(!(move.from <= 63  && move.to <= 63)) return false;


    Piece piece = game->board[move.from / 8][move.from % 8];
    Piece target = game->board[move.to / 8][move.to % 8];

    if (piece_colour(piece) != game->turn) return false; // cannot move on opponents turn
    if (piece == EMPTY) return false; // moving empty squares not allowed
    if (target != EMPTY && (piece_colour(piece) == piece_colour(target))) return false; // cannot capture own piece

    // peice movement logic
    switch (piece) {
        case BLACK_PAWN:
        case WHITE_PAWN:
            return pawnMovementChecker(game,move);

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
            return queenMovementChecker(game,move);

        case BLACK_KING:
        case WHITE_KING:
            return kingMovementChecker(move);
        }

    return false;
}

static bool recordHistory(Game *game, Move move){
    if (game->move_num >= MAX_MOVE_HISTORY)
        return false;

    Piece piece = game->board[move.from / 8][move.from % 8];
    Piece captured_piece = game->board[move.to / 8][move.to % 8];

    if (move.promotion != PROMOTE_NONE) { // promotions
        if (piece == WHITE_PAWN) {
            switch (move.promotion) {
                case PROMOTE_QUEEN:
                    piece = WHITE_QUEEN;
                    break;
                case PROMOTE_ROOK:
                    piece = WHITE_ROOK;
                    break;
                case PROMOTE_BISHOP:
                    piece = WHITE_BISHOP;
                    break;
                case PROMOTE_KNIGHT:
                    piece = WHITE_KNIGHT;
                    break;
                default:
                    break;
            }
        }
        else if (piece == BLACK_PAWN) {
            switch (move.promotion) {
                case PROMOTE_QUEEN:
                    piece = BLACK_QUEEN;
                    break;
                case PROMOTE_ROOK:
                    piece = BLACK_ROOK;
                    break;
                case PROMOTE_BISHOP:
                    piece = BLACK_BISHOP;
                    break;
                case PROMOTE_KNIGHT:
                    piece = BLACK_KNIGHT;
                    break;
                default:
                    break;
            }
        }
    }

    game->history[game->move_num].move = move;
    game->history[game->move_num].captured_piece = captured_piece;
    game->history[game->move_num].previous_turn = game->turn;

    game->board[move.to / 8][move.to % 8] = piece;
    game->board[move.from / 8][move.from % 8] = EMPTY;

    game->turn = (game->turn == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;

    game->move_num++;

    return true;
}

static bool isSquareAttacked(Game *game, int target, Colour attacker){ // checks if the attacker can move a piece from its current square to the target square
    for (int from = 0; from < 64; from++) {

        Piece piece = game->board[from / 8][from % 8];

        if (piece_colour(piece) != attacker)
            continue;

        Move move = { .from = from, .to = target}; // construct the move

        Colour previous_turn = game->turn;
        game->turn = attacker;

        if (islegalmove(game, move)) {
            game->turn = previous_turn;
            return true;
        }

        game->turn = previous_turn;
    }

    return false;
}

static bool isChecked(Game *game, Colour colour){ // finds the given colours king and checks wether its square is being attacked
    Piece king = (colour == COLOUR_WHITE) ? WHITE_KING : BLACK_KING;
    for (int square = 0; square < 64; square++) {
        if (game->board[square / 8][square % 8] == king) {
            Colour attacker = (colour == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;
            return isSquareAttacked(game, square, attacker);
        }
    }

    return false;
}

// HELPERS END

void Reset_game(Game *game){
    game->turn = COLOUR_WHITE;
    game->state = GAME_IN_PROGRESS;
    game->move_num = 0;

    init_board(game);
}

Game *Create_Game(void){
    Game *game = calloc(1, sizeof(Game));

    if (game == NULL)
        return NULL;

    Reset_game(game);

    return game;
}

void Destroy_Game(Game *game){
    free(game);
}

bool Make_Move(Game *game, Move move){

    if (!islegalmove(game, move)) // checks if the move is legal before recording it
        return false;

    return recordHistory(game, move); // record history records the move and performs it
}

void Undo_Move(Game *game){
    MoveHistory history = game->history[game->move_num - 1]; // grabs the last history of the last move
    Move move = history.move; // grabs the last move

    
    Piece piece = game->board[move.to / 8][move.to % 8]; // grabs the peice mobed

    // undos the move
    game->board[move.from / 8][move.from % 8] = piece;
    game->board[move.to / 8][move.to % 8] = history.captured_piece;

    game->turn = history.previous_turn;

    game->move_num--;
}

MoveList GenerateMoves(Game *game){ // generates a list of legal moves that dont put the king in check
    MoveList legal_moves = {0};

    Colour original_turn = game->turn;

    for (int from = 0; from < 64; from++) {
        for (int to = 0; to < 64; to++) {

            Move move = {.from = from, .to = to};

            if (!islegalmove(game, move))
                continue;

            Piece piece = game->board[move.from / 8][move.from % 8];

            bool promotion =
                (piece == WHITE_PAWN && move.to / 8 == 0) ||
                (piece == BLACK_PAWN && move.to / 8 == 7);

            if (promotion) {
                Promotion promotions[] = {
                    PROMOTE_QUEEN,
                    PROMOTE_ROOK,
                    PROMOTE_BISHOP,
                    PROMOTE_KNIGHT
                };

                for (int i = 0; i < 4; i++) {
                    Move promotion_move = move;
                    promotion_move.promotion = promotions[i];

                    if (!Make_Move(game, promotion_move))
                        continue;

                    if (!isChecked(game, original_turn)) {
                        legal_moves.moves[legal_moves.count] = promotion_move;
                        legal_moves.count++;
                    }

                    Undo_Move(game);
                }
            }
            else {
                if (!Make_Move(game, move))
                    continue;

                if (!isChecked(game, original_turn)) {
                    legal_moves.moves[legal_moves.count] = move;
                    legal_moves.count++;
                }

                Undo_Move(game);
            }
        }
    }
    return legal_moves;
}

