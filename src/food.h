#ifndef FOOD_H
#define FOOD_H

#include "types.h"
#include <time.h>

void set_max_food(Food *);
void check_food(Snake *, Food *, Timer *, Stats *);

#endif /* FOOD_H */