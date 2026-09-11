#include "engine.h"
#include "game.h"

#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

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
                case WHITE_PAWN:   score += 100; break;
                case WHITE_KNIGHT: score += 320; break;
                case WHITE_BISHOP: score += 330; break;
                case WHITE_ROOK:   score += 500; break;
                case WHITE_QUEEN:  score += 900; break;

                case BLACK_PAWN:   score -= 100; break;
                case BLACK_KNIGHT: score -= 320; break;
                case BLACK_BISHOP: score -= 330; break;
                case BLACK_ROOK:   score -= 500; break;
                case BLACK_QUEEN:  score -= 900; break;

                default:
                    break;
            }
        }
    }
    return score;
}

int minmax(Game *game,int depth,int alpha,int beta,bool maximizingPlayer){
    if(depth == 0) return evaluate(game);

    MoveList moves = GenerateMoves(game);
    if(maximizingPlayer){
        if(moves.count == 0){
            if(isChecked(game, COLOUR_WHITE)){
                return INT_MIN;
            }
            return 0;
        }
        int maxEval = INT_MIN;
        for (size_t i = 0; i < moves.count; i++) {
            Make_Move(game, moves.moves[i]);

            int score = minmax(game, depth - 1,alpha,beta,false);

            Undo_Move(game);
            
            maxEval = max(maxEval, score);
            alpha = max(alpha, score);
            if(beta <= alpha){
                break;
            }
        }

        return maxEval;
    }
    else{
        if(moves.count == 0){
            if(isChecked(game, COLOUR_BLACK)){
                return INT_MAX;
            }
            return 0;
        }
        int minEval = INT_MAX;
        for (size_t i = 0; i < moves.count; i++) {
            Make_Move(game, moves.moves[i]);

            int score = minmax(game, depth - 1,alpha,beta,true);

            Undo_Move(game);

            minEval = min(minEval, score);
            beta = min(beta, score);
            if(beta <= alpha){
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
    int best_score = INT_MIN;

    for (size_t i = 0; i < moves.count; i++) {
        Make_Move(game, moves.moves[i]);

        int score = minmax(game, 3, INT_MIN, INT_MAX, false);

        Undo_Move(game);

        if (score > best_score) {
            best_score = score;
            best_move = moves.moves[i];
        }
    }

    return best_move;
}
