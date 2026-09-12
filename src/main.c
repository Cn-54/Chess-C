#include "game.h"
#include "uci.h"
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int main(void){
    srand(time(NULL));

    Game *game = Create_Game();
    init_board(game);

    // thread the uci loop
    pthread_t uci_thread;

    pthread_create(&uci_thread, NULL, UCI_Loop, game);

    pthread_join(uci_thread, NULL);

    return 0;
}