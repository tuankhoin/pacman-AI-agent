#ifndef __AI__
#define __AI__

#include <stdint.h>
#include <unistd.h>
#include "node.h"
#include "priority_queue.h"

#define INVINCIBLE 10
#define LIFE_LOST 10
#define GAME_OVER 100 

void initialize_ai();

move_t get_next_move( state_t init_state, int budget, propagation_t propagation, char* stats ,move_t recent_move);

#endif
