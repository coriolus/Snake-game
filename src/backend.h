#ifndef BACKEND_H
#define BACKEND_H

#include "types.h"

#define MAX_LIVES 5
#define CHECK_SNAKE 0
#define CHECK_FOOD 1

void run_game(void);
int is_snake(Snake *, Position, int);
Eats * is_food(Food *, Position);
int is_same_position(Position, Position);

#endif /* BACKEND_H */
