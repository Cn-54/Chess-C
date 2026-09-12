#ifndef ENGINE_H
#define ENGINE_H

#include "game.h"
#include <stddef.h>

extern volatile bool stop_requested;

Move Think(Game *game);

#endif