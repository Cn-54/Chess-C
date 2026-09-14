#include "engine.h"
#include "game.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

volatile bool stop_requested = false;

#define DEPTH 5 // max depth
#define QUIESCENCE_DEPTH 10

#define MATE_SCORE 1000000
#define MATE_DISTANCE_SCALE 20

static long long nodes = 0;

static int pieceValue(Piece piece){ // holds the value of each peice
    switch (piece) {
        case WHITE_PAWN:
        case BLACK_PAWN:
            return 100;

        case WHITE_KNIGHT:
        case BLACK_KNIGHT:
            return 320;

        case WHITE_BISHOP:
        case BLACK_BISHOP:
            return 330;

        case WHITE_ROOK:
        case BLACK_ROOK:
            return 500;

        case WHITE_QUEEN:
        case BLACK_QUEEN:
            return 900;

        case WHITE_KING:
        case BLACK_KING:
            return 20000;

        default:
            return 0;
    }
}

static int moveScore(Game *game, Move move){
    int score = 0;

    Piece attacker =
        game->board[move.from / 8][move.from % 8];

    Piece victim =
        game->board[move.to / 8][move.to % 8];

    // Captures
    if (victim != EMPTY) {
        score += pieceValue(victim) - pieceValue(attacker);
    }

    // Promotions
    if (move.promotion != PROMOTE_NONE) {

        switch (move.promotion) {
            case PROMOTE_QUEEN:
                score += pieceValue(WHITE_QUEEN); // colour doesnt matter same value
                break;
            case PROMOTE_ROOK:
                score += pieceValue((WHITE_ROOK));
                break;
            case PROMOTE_BISHOP:
                score += pieceValue(WHITE_BISHOP);
                break;
            case PROMOTE_KNIGHT:
                score += pieceValue(WHITE_KNIGHT);
                break;
            default:
                break;
        }
    }

    return score;
}

static void orderMoves(Game *game, MoveList *moves){ // Sort the moves list so better moves come first
    for (size_t i = 1; i < moves->count; i++) {
        Move current = moves->moves[i];
        int currentScore = moveScore(game, current);

        int j = i - 1;

        while (j >= 0 &&
               moveScore(game, moves->moves[j]) < currentScore) {

            moves->moves[j + 1] = moves->moves[j];
            j--;
        }

        moves->moves[j + 1] = current;
    }
}

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
                            score += pieceValue(WHITE_PAWN) + pawn_table[y][x];
                            break;

                        case WHITE_KNIGHT:
                            score += pieceValue(WHITE_KNIGHT) + knight_table[y][x];
                            break;

                        case WHITE_BISHOP:
                            score += pieceValue(WHITE_BISHOP) + bishop_table[y][x];
                            break;

                        case WHITE_ROOK:
                            score += pieceValue(WHITE_ROOK) + rook_table[y][x];
                            break;

                        case WHITE_QUEEN:
                            score += pieceValue(WHITE_QUEEN) + queen_table[y][x];
                            break;
                        case WHITE_KING:
                            score += king_table[y][x];
                            break;

                        case BLACK_PAWN:
                            score -= pieceValue(BLACK_PAWN) + pawn_table[7-y][x]; // 7-y flips the table for black
                            break;

                        case BLACK_KNIGHT:
                            score -= pieceValue(BLACK_KNIGHT) + knight_table[7-y][x];
                            break;

                        case BLACK_BISHOP:
                            score -= pieceValue(BLACK_BISHOP) + bishop_table[7-y][x];
                            break;

                        case BLACK_ROOK:
                            score -= pieceValue(BLACK_ROOK) + rook_table[7-y][x];
                            break;

                        case BLACK_QUEEN:
                            score -= pieceValue(BLACK_QUEEN) + queen_table[7-y][x];
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
    int to_y = move.to / 8;
    int to_x = move.to % 8;

    if (game->board[to_y][to_x] != EMPTY)
        return true;

    return move.to == game->en_passant; // allow en passant captures
}

int quiescence(Game *game, int alpha, int beta,bool maximizingPlayer, int depth){
    nodes++;

    if (stop_requested) return evaluate(game);

    bool in_check = isChecked(game, game->turn);
    int stand_pat = evaluate(game);

    if (!in_check) {
        if (maximizingPlayer) {
            if (stand_pat >= beta)
                return beta;

            if (stand_pat > alpha)
                alpha = stand_pat;
        }
        else {
            if (stand_pat <= alpha)
                return alpha;

            if (stand_pat < beta)
                beta = stand_pat;
        }
    }

    if (depth == 0)
        return in_check ? evaluate(game) :
               (maximizingPlayer ? alpha : beta);

    MoveList moves = GenerateMoves(game);

    if (moves.count == 0) {
        if (in_check) {
            if (game->turn == COLOUR_WHITE)
                return -MATE_SCORE;
            else
                return MATE_SCORE;
        }

        return 0; // stalemate
    }

    orderMoves(game, &moves);

    for (size_t i = 0; i < moves.count; i++) {
        Move move = moves.moves[i];

        if (!in_check &&
            !isCapture(game, move) &&
            move.promotion == PROMOTE_NONE)
            continue;

        Make_Move(game, move);

        int score = quiescence(
            game,
            alpha,
            beta,
            !maximizingPlayer,
            depth - 1
        );

        Undo_Move(game);

        if (maximizingPlayer) {
            if (score > alpha)
                alpha = score;

            if (alpha >= beta)
                break;
        }
        else {
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

int minmax(Game *game, int depth, int alpha, int beta, bool maximizingPlayer,int piles){
    nodes++;
    if (stop_requested)
        return evaluate(game);
    if (isRepetition(game)) return 0;
    if (depth == 0)
        return quiescence(game, alpha, beta, maximizingPlayer, QUIESCENCE_DEPTH);

    MoveList moves = GenerateMoves(game);
    orderMoves(game, &moves);

    if (maximizingPlayer){

        if (moves.count == 0){
            if (isChecked(game, COLOUR_WHITE)){
                return -MATE_SCORE + (piles * MATE_DISTANCE_SCALE);
            }
            return 0;
        }

        int maxEval = INT_MIN;

        for (size_t i = 0; i < moves.count; i++) {

            Make_Move(game, moves.moves[i]);
            
            int score = minmax(game, depth - 1, alpha, beta, false,piles+1);

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
                return MATE_SCORE - (piles * MATE_DISTANCE_SCALE);
            }
            return 0;
        }
        int minEval = INT_MAX;

        for (size_t i = 0; i < moves.count; i++) {

            Make_Move(game, moves.moves[i]);

            int score = minmax(game, depth - 1, alpha, beta, true,piles+1);

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
    nodes = 0;

    MoveList moves = GenerateMoves(game); // generates all legal moves

    if (moves.count == 0) // if there are no moves then return
        return (Move){0};

    Colour side = game->turn;

    Move best_move = moves.moves[0];

    for (int depth = 1; depth <= DEPTH; depth++) { // iterative deepening

        int best_score = (side == COLOUR_WHITE) ? INT_MIN : INT_MAX;

        Move depth_best_move = best_move;

        for (size_t i = 0; i < moves.count; i++) { // for all possible moves

            Make_Move(game, moves.moves[i]); // make the root move

            int score = minmax( game,depth - 1,INT_MIN,INT_MAX,game->turn == COLOUR_WHITE,1); // evaulate the game from the root move

            Undo_Move(game); // undo that move

            if (stop_requested)
                break;

            if (side == COLOUR_WHITE) { // maximise for white
                if (score > best_score) {
                    best_score = score;
                    depth_best_move = moves.moves[i];
                }
            }
            else { // minimise for black
                if (score < best_score) {
                    best_score = score;
                    depth_best_move = moves.moves[i];
                }
            }
        }

        if (stop_requested)
            break;

        best_move = depth_best_move;

        printf("info depth %d score cp %d nodes %lld\n",
               depth, best_score, nodes);
        fflush(stdout);
    }

    return best_move;
}