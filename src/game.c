#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

static bool isSquareAttacked(Game *game, int target, Colour attacker);

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

static bool kingMovementChecker(Game *game, Move move){
    int from_y = move.from / 8;
    int from_x = move.from % 8;
    int to_y = move.to / 8;
    int to_x = move.to % 8;

    int distX = abs(to_x - from_x);
    int distY = abs(to_y - from_y);

    // Normal king move
    if (distX <= 1 && distY <= 1)
        return true;

    // Castling
    if (distY != 0 || distX != 2)
        return false;

    if (game->turn == COLOUR_WHITE) {

        // White kingside
        if (move.from == 60 && move.to == 62) {
            if (!game->castling.white_kingside)
                return false;

            if (game->board[7][7] != WHITE_ROOK)
                return false;

            if (game->board[7][5] != EMPTY ||
                game->board[7][6] != EMPTY)
                return false;

            if (isChecked(game, COLOUR_WHITE))
                return false;

            if (isSquareAttacked(game, 60, COLOUR_BLACK) ||
                isSquareAttacked(game, 61, COLOUR_BLACK) ||
                isSquareAttacked(game, 62, COLOUR_BLACK))
                return false;
            return true;
        }

        // White queenside
        if (move.from == 60 && move.to == 58) {
            if (!game->castling.white_queenside)
                return false;

            if (game->board[7][0] != WHITE_ROOK)
                return false;

            if (game->board[7][1] != EMPTY ||
                game->board[7][2] != EMPTY ||
                game->board[7][3] != EMPTY)
                return false;

            if (isChecked(game, COLOUR_WHITE))
                return false;

            if(isSquareAttacked(game, 60, COLOUR_BLACK) ||
                isSquareAttacked(game, 59, COLOUR_BLACK) ||
                isSquareAttacked(game, 58, COLOUR_BLACK)) return false;
            return true;
        }
    }

    else {

        // Black kingside
        if (move.from == 4 && move.to == 6) {
            if (!game->castling.black_kingside)
                return false;

            if (game->board[0][7] != BLACK_ROOK)
                return false;

            if (game->board[0][5] != EMPTY ||
                game->board[0][6] != EMPTY)
                return false;

            if (isChecked(game, COLOUR_BLACK))
                return false;

            if(isSquareAttacked(game, 4, COLOUR_WHITE) ||
                isSquareAttacked(game, 5, COLOUR_WHITE) ||
                isSquareAttacked(game, 6, COLOUR_WHITE)) return false;
            return true;
        }

        // Black queenside
        if (move.from == 4 && move.to == 2) {
            if (!game->castling.black_queenside)
                return false;

            if (game->board[0][0] != BLACK_ROOK)
                return false;

            if (game->board[0][1] != EMPTY ||
                game->board[0][2] != EMPTY ||
                game->board[0][3] != EMPTY)
                return false;

            if (isChecked(game, COLOUR_BLACK))
                return false;

            if(isSquareAttacked(game, 4, COLOUR_WHITE) ||
                isSquareAttacked(game, 3, COLOUR_WHITE) ||
                isSquareAttacked(game, 2, COLOUR_WHITE)) return false;
            return true;
        }
    }

    return false;
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

    // Normal capture
    if (distX == 1 && to_y == from_y + direction) {

        if (target != EMPTY &&
            piece_colour(target) != game->turn) {
            return true;
        }

        // En passant
        if (target == EMPTY && move.to == game->en_passant) {

            int captured_y = to_y - direction;

            Piece captured = game->board[captured_y][to_x];

            return (game->turn == COLOUR_WHITE && captured == BLACK_PAWN) || (game->turn == COLOUR_BLACK && captured == WHITE_PAWN);
        }
    }

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
            return kingMovementChecker(game,move);
        }

    return false;
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



static bool recordHistory(Game *game, Move move){
    if (game->move_num >= MAX_MOVE_HISTORY)
        return false;

    Piece original_piece =
        game->board[move.from / 8][move.from % 8];

    Piece captured_piece =
        game->board[move.to / 8][move.to % 8];

    // Save state before making the move
    game->history[game->move_num].move = move;
    game->history[game->move_num].moved_piece = original_piece;
    game->history[game->move_num].captured_piece = captured_piece;
    game->history[game->move_num].previous_turn = game->turn;

    game->history[game->move_num].previous_en_passant =
        game->en_passant;

    game->history[game->move_num].previous_castling =
        game->castling;

    game->history[game->move_num].was_en_passant = false;
    game->history[game->move_num].was_castling = false;


    // en pasaant

    bool en_passant_capture = false;

    if ((original_piece == WHITE_PAWN ||
         original_piece == BLACK_PAWN) &&

        move.to == game->en_passant &&

        captured_piece == EMPTY) {

        en_passant_capture = true;

        game->history[game->move_num].was_en_passant = true;

        int direction =
            (original_piece == WHITE_PAWN) ? -1 : 1;

        int captured_y =
            move.to / 8 - direction;

        captured_piece =
            game->board[captured_y][move.to % 8];

        // Store the pawn that was actually captured
        game->history[game->move_num].captured_piece =
            captured_piece;
    }


    // castling
    bool castling = false;

    if (original_piece == WHITE_KING &&
        (move.from == 60 && (move.to == 62 || move.to == 58))) {
        castling = true;
    }
    else if (original_piece == BLACK_KING &&
            (move.from == 4 && (move.to == 6 || move.to == 2))) {
        castling = true;
    }

    game->history[game->move_num].was_castling = castling;


    // promotion

    Piece piece = original_piece;

    if (move.promotion != PROMOTE_NONE) {

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


   // move peices
    game->board[move.from / 8][move.from % 8] = EMPTY;

    game->board[move.to / 8][move.to % 8] = piece;


    // removed en passant campure

    if (en_passant_capture) {

        int direction =
            (original_piece == WHITE_PAWN) ? -1 : 1;

        int captured_y =
            move.to / 8 - direction;

        game->board[captured_y][move.to % 8] = EMPTY;
    }


    // move rook when castlin

    if (castling) {

        // White kingside
        if (move.from == 60 && move.to == 62) {

            game->board[7][5] =
                game->board[7][7];

            game->board[7][7] = EMPTY;
        }

        // White queenside
        else if (move.from == 60 && move.to == 58) {

            game->board[7][3] =
                game->board[7][0];

            game->board[7][0] = EMPTY;
        }

        // Black kingside
        else if (move.from == 4 && move.to == 6) {

            game->board[0][5] =
                game->board[0][7];

            game->board[0][7] = EMPTY;
        }

        // Black queenside
        else if (move.from == 4 && move.to == 2) {

            game->board[0][3] =
                game->board[0][0];

            game->board[0][0] = EMPTY;
        }
    }


    //update the castling rights

    // King moved
    if (original_piece == WHITE_KING) {

        game->castling.white_kingside = false;
        game->castling.white_queenside = false;
    }

    else if (original_piece == BLACK_KING) {

        game->castling.black_kingside = false;
        game->castling.black_queenside = false;
    }


    // Rook moved
    if (original_piece == WHITE_ROOK) {

        if (move.from == 63)
            game->castling.white_kingside = false;

        else if (move.from == 56)
            game->castling.white_queenside = false;
    }

    else if (original_piece == BLACK_ROOK) {

        if (move.from == 7)
            game->castling.black_kingside = false;

        else if (move.from == 0)
            game->castling.black_queenside = false;
    }


    // crook captured

    if (captured_piece == WHITE_ROOK) {

        if (move.to == 63)
            game->castling.white_kingside = false;

        else if (move.to == 56)
            game->castling.white_queenside = false;
    }

    else if (captured_piece == BLACK_ROOK) {

        if (move.to == 7)
            game->castling.black_kingside = false;

        else if (move.to == 0)
            game->castling.black_queenside = false;
    }


    // update the en passant

    game->en_passant = -1;


    // White pawn moved two squares
    if (original_piece == WHITE_PAWN &&
        move.from / 8 == 6 &&
        move.to / 8 == 4) {

        game->en_passant = move.from - 8;
    }


    // Black pawn moved two squares
    else if (original_piece == BLACK_PAWN && move.from / 8 == 1 && move.to / 8 == 3) {
        game->en_passant = move.from + 8;
    }


    // change turn

    game->turn = (game->turn == COLOUR_WHITE) ? COLOUR_BLACK : COLOUR_WHITE;

    game->move_num++;

    return true;
}



bool isChecked(Game *game, Colour colour){ // finds the given colours king and checks wether its square is being attacked
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

    game->castling.white_kingside = true;
    game->castling.white_queenside = true;
    game->castling.black_kingside = true;
    game->castling.black_queenside = true;

    game->en_passant = -1;

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
    if (game->move_num <= 0)
        return;

    MoveHistory history =
        game->history[game->move_num - 1];

    Move move = history.move;


    // restore previous game state

    game->en_passant =
        history.previous_en_passant;

    game->castling =
        history.previous_castling;

    game->turn =
        history.previous_turn;


    // resotre moved piece

    game->board[move.from / 8][move.from % 8] =
        history.moved_piece;


    // restore peices

    game->board[move.to / 8][move.to % 8] =
        history.captured_piece;


    // undo en passant

    if (history.was_en_passant) {

        int direction =
            (history.moved_piece == WHITE_PAWN)
            ? -1
            : 1;

        int captured_y =
            move.to / 8 - direction;

        // Destination was originally empty
        game->board[move.to / 8][move.to % 8] =
            EMPTY;

        // Put captured pawn back
        game->board[captured_y][move.to % 8] =
            history.captured_piece;
    }


    // undo castling

    if (history.was_castling) {

        // White kingside
        if (move.from == 60 && move.to == 62) {

            game->board[7][7] =
                game->board[7][5];

            game->board[7][5] = EMPTY;
        }

        // White queenside
        else if (move.from == 60 && move.to == 58) {

            game->board[7][0] =
                game->board[7][3];

            game->board[7][3] = EMPTY;
        }

        // Black kingside
        else if (move.from == 4 && move.to == 6) {

            game->board[0][7] =
                game->board[0][5];

            game->board[0][5] = EMPTY;
        }

        // Black queenside
        else if (move.from == 4 && move.to == 2) {

            game->board[0][0] =
                game->board[0][3];

            game->board[0][3] = EMPTY;
        }
    }


    game->move_num--;
}

MoveList GenerateMoves(Game *game){ // generates a list of legal moves that dont put the king in check

    MoveList legal_moves = {0};
    Colour original_turn = game->turn;

    for (int from = 0; from < 64; from++) {

        Piece piece = game->board[from / 8][from % 8];

        if (piece == EMPTY)
            continue;

        if (piece_colour(piece) != game->turn)
            continue;

        for (int to = 0; to < 64; to++) {

            if (to == from)
                continue;

            Move move = {.from = from, .to = to};

            if (!islegalmove(game, move))
                continue;

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

                    recordHistory(game, promotion_move);

                    if (!isChecked(game, original_turn)) {
                        legal_moves.moves[legal_moves.count] = promotion_move;
                        legal_moves.count++;
                    }

                    Undo_Move(game);
                }
            }
            else {

                recordHistory(game, move);

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