#include "game.h"
#include "uci.h"

int main(void)
{
    Game *game = Create_Game();

    init_board(game);
    UCI_Loop(game);

    return 0;
}