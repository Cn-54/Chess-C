#include "engine.h"

#include <stdlib.h>

Move Think(Game *game){
    MoveList moves = GenerateMoves(game);

    if (moves.count == 0)
        return (Move){0};

    return moves.moves[rand() % moves.count];
}
