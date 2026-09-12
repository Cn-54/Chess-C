#include "engine.h"
#include "game.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

volatile bool stop_requested = false;

#define DEPTH 2 // effective depth is DEPTH+1
#define QUIESCENCE_DEPTH 6

static const int pawn_table[8][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0},
    { 50,  50,  50,  50,  50,  50,  50,  50},
    { 10,  10,  20,  30,  30,  20,  10,  10},
    {  5,   5,  10,  25,  25,  10,   5,   5},
    {  0,   0,   0,  20,  20,   0,   0,   0},
    {  5,  -5, -10,   0,   0, -10,  -5,   5},
    {  5,  10,  10, -25, -25,  10,  10,   5},
    {  0,   0,   0,   0,   0,   0,   0,   0}
};

static const int knight_table[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20,   0,   0,   0,   0, -20, -40},
    {-30,   0,  10,  15,  15,  10,   0, -30},
    {-30,   5,  15,  20,  20,  15,   5, -30},
    {-30,   0,  15,  20,  20,  15,   0, -30},
    {-30,   5,  10,  15,  15,  10,   5, -30},
    {-40, -20,   0,   5,   5,   0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50}
};

static const int bishop_table[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10,   0,   0,   0,   0,   0,   0, -10},
    {-10,   0,   10,  10,  10,  10,   0, -10},
    {-10,   5,   5,  10,  10,   5,   5, -10},
    {-10,   0,   10,  10,  10,  10,   0, -10},
    {-10,  10,  10,  10,  10,  10,  10, -10},
    {-10,   5,   0,   0,   0,   0,   5, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20}
};

static const int rook_table[8][8] = {
    {  0,   0,   0,   5,   5,   0,   0,   0},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    { -5,   0,   0,   0,   0,   0,   0,  -5},
    {  5,  10,  10,  10,  10,  10,  10,   5},
    {  0,   0,   0,   0,   0,   0,   0,   0}
};

static const int queen_table[8][8] = {
    {-20, -10, -10,  -5,  -5, -10, -10, -20},
    {-10,   0,   0,   0,   0,   0,   0, -10},
    {-10,   0,   5,   5,   5,   5,   0, -10},
    { -5,   0,   5,   5,   5,   5,   0,  -5},
    {  0,   0,   5,   5,   5,   5,   0,  -5},
    {-10,   5,   5,   5,   5,   5,   5, -10},
    {-10,   0,   5,   0,   0,   0,   0, -10},
    {-20, -10, -10,  -5,  -5, -10, -10, -20}
};

static const int king_table[8][8] = {
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-30, -40, -40, -50, -50, -40, -40, -30},
    {-20, -30, -30, -40, -40, -30, -30, -20},
    {-10, -20, -20, -20, -20, -20, -20, -10},
    { 20,  20,   0,   0,   0,   0,  20,  20},
    { 20,  30,  10,   0,   0,  10,  30,  20}
};


static int max(int a, int b){
    if(a > b){
        return a;
    }
    return b;
}
static int min(int a, int b){
    if(a < b){
        return a;
    }
    return b;
}


int evaluate(Game *game){
    int score =0;
    for(size_t y = 0; y < 8; y++){
        for(size_t x = 0; x < 8; x++){
            Piece piece = game->board[y][x];

                    switch (piece) {
                        case WHITE_PAWN:
                            score += 100 + pawn_table[y][x];
                            break;

                        case WHITE_KNIGHT:
                            score += 320 + knight_table[y][x];
                            break;

                        case WHITE_BISHOP:
                            score += 330 + bishop_table[y][x];
                            break;

                        case WHITE_ROOK:
                            score += 500 + rook_table[y][x];
                            break;

                        case WHITE_QUEEN:
                            score += 900 + queen_table[y][x];
                            break;
                        case WHITE_KING:
                            score += king_table[y][x];
                            break;

                        case BLACK_PAWN:
                            score -= 100 + pawn_table[7-y][x]; // 7-y flips the table for black
                            break;

                        case BLACK_KNIGHT:
                            score -= 320 + knight_table[7-y][x];
                            break;

                        case BLACK_BISHOP:
                            score -= 330 + bishop_table[7-y][x];
                            break;

                        case BLACK_ROOK:
                            score -= 500 + rook_table[7-y][x];
                            break;

                        case BLACK_QUEEN:
                            score -= 900 + queen_table[7-y][x];
                            break;
                        case BLACK_KING:
                            score -= king_table[7-y][x];
                            break;


                default:
                    break;
            }
        }
    }
    return score;
}

static bool isCapture(Game *game, Move move){
    return game->board[move.to / 8][move.to % 8] != EMPTY;
}

int quiescence(Game *game, int alpha, int beta, bool maximizingPlayer, int depth){
    if (stop_requested)
        return evaluate(game);
    int stand_pat = evaluate(game);
    if (depth == 0)
        return stand_pat;

    if (maximizingPlayer) {
        if (stand_pat >= beta)
            return beta;

        if (stand_pat > alpha)
            alpha = stand_pat;
    } else {
        if (stand_pat <= alpha)
            return alpha;

        if (stand_pat < beta)
            beta = stand_pat;
    }

    MoveList moves = GenerateMoves(game);

    for (size_t i = 0; i < moves.count; i++) {
        Move move = moves.moves[i];

        if (!isCapture(game, move))
            continue;

        Make_Move(game, move);

        int score = quiescence(game,alpha,beta,!maximizingPlayer,depth-1);

        Undo_Move(game);

        if (maximizingPlayer) {
            if (score > alpha)
                alpha = score;

            if (alpha >= beta)
                break;
        } else {
            if (score < beta)
                beta = score;

            if (beta <= alpha)
                break;
        }
    }

    return maximizingPlayer ? alpha : beta;
}

bool isRepetition(Game *game){
    if (game->move_num < 4)
        return 0;

    Move current_a = game->history[game->move_num - 2].move;
    Move current_b = game->history[game->move_num - 1].move;

    for (int i = game->move_num - 4; i >= 0; i -= 2) {

        Move a = game->history[i].move;
        Move b = game->history[i + 1].move;

        if (a.from == current_a.from &&
            a.to   == current_a.to &&
            b.from == current_b.from &&
            b.to   == current_b.to) {

            return true;
        }
    }

    return false;
}

int minmax(Game *game, int depth, int alpha, int beta, bool maximizingPlayer){
    if (stop_requested)
        return evaluate(game);
    if (isRepetition(game)) return 0;
    if (depth == 0)
        return quiescence(game, alpha, beta, maximizingPlayer, QUIESCENCE_DEPTH);

    MoveList moves = GenerateMoves(game);

    if (maximizingPlayer){

        if (moves.count == 0){
            if (isChecked(game, COLOUR_WHITE)){
                return INT_MIN;
            }
            return 0;
        }

        int maxEval = INT_MIN;

        for (size_t i = 0; i < moves.count; i++) {

            Make_Move(game, moves.moves[i]);

            int score = minmax(game, depth - 1, alpha, beta, false);

            Undo_Move(game);

            maxEval = max(maxEval, score);
            alpha = max(alpha, score);

            if (beta <= alpha) {
                break;
            }
        }

        return maxEval;
    }

    else{

        if (moves.count == 0){
            if (isChecked(game, COLOUR_BLACK)){
                return INT_MAX;
            }
            return 0;
        }
        int minEval = INT_MAX;

        for (size_t i = 0; i < moves.count; i++) {

            Make_Move(game, moves.moves[i]);

            int score = minmax(game, depth - 1, alpha, beta, true);

            Undo_Move(game);

            minEval = min(minEval, score);
            beta = min(beta, score);

            if (beta <= alpha) {
                break;
            }
        }

        return minEval;
    }
}


Move Think(Game *game){
    MoveList moves = GenerateMoves(game);

    if (moves.count == 0)
        return (Move){0};

    Move best_move = moves.moves[0];

    if (game->turn == COLOUR_WHITE) {

        int best_score = INT_MIN;

        for (size_t i = 0; i < moves.count; i++) {
            Make_Move(game, moves.moves[i]);

            int score = minmax(game, DEPTH, INT_MIN, INT_MAX, false);

            Undo_Move(game);

            if (stop_requested)
                break;

            if (score > best_score) {
                best_score = score;
                best_move = moves.moves[i];
            }
        }

    } else {

        int best_score = INT_MAX;

        for (size_t i = 0; i < moves.count; i++) {
            Make_Move(game, moves.moves[i]);

            int score = minmax(game, DEPTH, INT_MIN, INT_MAX, true);

            Undo_Move(game);

            if (stop_requested)
                break;

            if (score < best_score) {
                best_score = score;
                best_move = moves.moves[i];
            }
        }
    }

    return best_move;
}
