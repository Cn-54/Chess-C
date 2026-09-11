#include "game.h"
#include "uci.h"
#include <stdlib.h>
#include <time.h>


int main(void){
    srand(time(NULL));
    Game *game = Create_Game();

    init_board(game);
    UCI_Loop(game);

    return 0;
}