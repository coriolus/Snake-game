#define _CRT_SECURE_NO_WARNINGS
#include "food.h"
#include "frontend.h"
#include "backend.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_FOOD_1 5
#define MAX_FOOD_2 10
#define MAX_FOOD_3 15
#define CONSOLE_SIZE 80*25

#define DELAY_MIN_STD 1
#define DELAY_MAX_STD 5
#define DELAY_MIN_SP 40
#define DELAY_MAX_SP 120

#define STD_FOOD 'O'
#define VAL_FOOD '$'
#define HID_FOOD '?'

int set_random_delay(int, int);
void add_food(Snake *, Food *, FoodType, Timer *, Stats*);
int set_modifier(Food *, Timer *, Stats *);
Position set_food_position(Snake *, Food *);
int is_food_collision(Food *, Position);
void display_food(Eats *);
void remove_food(Food *, Eats *);

// define max food according to the size of the console window
void set_max_food(Food *food)
{
    Border border = get_border_coordinates();

    int width = border.right - border.left - 1;  
    int height = border.bottom - border.top - 1; 

    if (width * height < CONSOLE_SIZE)
        food->maxFood = MAX_FOOD_1;
    else if (width * height < CONSOLE_SIZE * 2)
        food->maxFood = MAX_FOOD_2;
    else
        food->maxFood = MAX_FOOD_3;
}

// get some random delay value 
int set_random_delay(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

// check food status and add or remove if necessary
void check_food(Snake *snake, Food *food, Timer *timer, Stats *stats)
{   
    if ((!food->stdCount && !food->foodToAdd) || (food->foodToAdd && timer->elapsed - timer->resetStd > timer->delay[stdStart]))
    {
        add_food(snake, food, standard, timer, stats);
        timer->delay[stdStart] = set_random_delay(DELAY_MIN_STD, DELAY_MAX_STD);
        timer->resetStd = timer->elapsed;
    }
    /* by default, special types of food (value and hidden) should appear in
    longer intervals and should disapper if not collected in time */
    if (!food->valCount && timer->elapsed - timer->resetVal > timer->delay[valStart])
    {
        add_food(snake, food, value, timer, stats);
        timer->delay[valStart] = set_random_delay(DELAY_MIN_SP, DELAY_MAX_SP);
        timer->resetVal = timer->elapsed;
    }
    else if (food->valCount && timer->elapsed - timer->resetVal > timer->delay[valEnd])
        remove_food(food, &food->value);

    if (!food->hidCount && timer->elapsed - timer->resetHid > timer->delay[hidStart])
    {
        add_food(snake, food, hidden, timer, stats);
        timer->delay[hidStart] = set_random_delay(DELAY_MIN_SP * 2, DELAY_MAX_SP * 2);
        timer->resetHid = timer->elapsed;
    }
    else if (food->hidCount && timer->elapsed - timer->resetHid > timer->delay[hidEnd])
        remove_food(food, &food->hidden);
}

void add_food(Snake *snake, Food *food, FoodType ftype, Timer *timer, Stats *stats)
{
    if (ftype == standard)
    {
        /* by default, 2/3 of time a single food item will be added,
         and 1/3 of the time a group of items will be added */
        if (!food->stdCount && !food->foodToAdd)
        {
            if (rand() % 3 == 0)
                food->foodToAdd = rand() % (food->maxFood - 1) + 2;
            else
                food->foodToAdd = 1;
        }

        // new food will be stored in the first empty position in the array
        if (food->foodToAdd)
        {   
            int i;
            for (i = 0; food->standard[i].pos.x != 0; i++);
            
            food->standard[i].pos = set_food_position(snake, food);
            food->standard[i].time = timer->elapsed;
            display_food(&food->standard[i]);
            food->stdCount++;
            food->foodToAdd--;
        }
    }
    // special types of food (value and hidden) appear individually and at random intervals
    else if (ftype == value)
    {
        food->value.pos = set_food_position(snake, food);
        display_food(&food->value);
        food->valCount = 1;
    }
    else if (ftype == hidden)
    {
        food->hidden.mod = set_modifier(food, timer, stats);
        food->hidden.pos = set_food_position(snake, food);
        display_food(&food->hidden);
        food->hidCount = 1;
    }
}

// get some random number to set the modifier
int set_modifier(Food *food, Timer *timer, Stats *stats)
{
    if (stats->lives < MAX_LIVES && !stats->lifeAdded)
        return rand() % 3;
    else
        return rand() % 2;
}

// set food position
Position set_food_position(Snake *snake, Food *food)
{
    Position pos;

    Border border = get_border_coordinates();
    
    // get some random coordinates and check if they are available
    do
    {   // we accept only even x coordinates since our snake segment is 2 chars wide
        while ((pos.x = (rand() % (border.right - border.left - 1) + border.left + 1)) % 2 == 1);
        pos.y = rand() % (border.bottom - border.top - 1) + border.top + 1;
    } while (is_food(food, pos) || is_snake(snake, pos, CHECK_FOOD));

    return pos; 
}

// display food on the screen
void display_food(Eats *eats)
{
    move_cursor(eats->pos.x, eats->pos.y);

    if (eats->ftype == standard)
        printf("%s%c%s", BRIGHT_BLUE, STD_FOOD, RESET_COLOR);
    else if (eats->ftype == value)
        printf("%s%c%s", YELLOW, VAL_FOOD, RESET_COLOR);
    else if (eats->ftype == hidden)
        printf("%s%c%s", RED, HID_FOOD, RESET_COLOR);
}

// remove food from the screen
void remove_food(Food *food, Eats *eats)
{
    move_cursor(eats->pos.x, eats->pos.y);
    printf("%c%s", ' ', RESET_COLOR);

    if (eats->ftype == standard)
        food->stdCount--;
    else if (eats->ftype == value)
        food->valCount = 0;
    else if (eats->ftype == hidden)   
        food->hidCount = 0;
}