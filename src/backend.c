#define _CRT_SECURE_NO_WARNINGS
#include "backend.h"
#include "frontend.h"
#include "food.h"
#include "menu.h"
#include "stats.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define START_STD 0
#define START_VAL 45
#define START_HID 30
#define REMOVE_STD 20
#define REMOVE_VAL 30
#define REMOVE_HID 30

#define BLOCK_SIZE 10
#define SEGMENT_SIZE 2
#define SEGMENT_COUNT 10
#define SNAKE_SIZE SEGMENT_SIZE*SEGMENT_COUNT
#define MAX_EVENTS 10

#define NORMAL 100
#define SLOW 175
#define FAST 50
#define START_DELAY 200
#define SPEED_DELAY 15
#define LIFE_DELAY 3

#define start_timer(X) time(&X)
#define stop_timer(X) time(&X)

void init_snake(Snake *);
void allocate_memory(Snake *, int);
void set_starting_position(Snake *);
void display_starting_position(Snake *);

void move_snake(Snake *, Food *, Timer *, Stats *); 

void check_modifier(Snake *, Timer *, Stats *);
Direction get_next_move(Direction, Stats *);
void game_paused(void);
void change_head_position(Snake *, int);
void change_tail_position(Snake *);
int set_tail_direction(Snake *);
void remove_segment(Snake *);
void display_new_position(Snake *, int);

int is_border(Snake *);
int is_intersection(Position, Position, Position);

void clear_board(void);
void display_game_over_msg(void);

void run_game(void)
{
    Stats st = {.score = 0};
    Stats *stats = &st;

    set_default_stats(stats, MAX_LIVES);

    do
    {
        Snake sn = {.speed = NORMAL, .dir = right};
        Snake *snake = &sn;
        Food fd = {.standard[0].ftype = standard, .value.ftype = value, .hidden.ftype = hidden};
        Food *food = &fd;
        Timer tm = {.delay = {START_STD, REMOVE_STD, START_VAL, REMOVE_VAL, START_HID, REMOVE_HID}};
        Timer *timer = &tm;
            
        srand(time(NULL));
        
        set_max_food(food);
        init_snake(snake);
        Sleep(START_DELAY);

        move_snake(snake, food, timer, stats);

        if (stats->hiscore > get_hiscore())
            save_hiscore(stats);

    }   while (stats->lives);

    display_game_over_msg();
}

void init_snake(Snake *snake)
{
    allocate_memory(snake, BLOCK_SIZE);
    set_starting_position(snake);
    display_starting_position(snake);
}

// allocate initial memory and reallocate when necessary
void allocate_memory(Snake *snake, int blockSize)
{
    Position *temp = (Position *) realloc(snake->seg, sizeof(Position) * blockSize);

    if (temp == NULL)
        exit(EXIT_FAILURE);
    else
    {
        snake->seg = temp;
        snake->allocatedMemory = blockSize;
    }
}

// set starting position
void set_starting_position(Snake *snake)
{    
    Window window = get_window_size();

    // adjust starting position on the x axis
    int startingPosition = window.cols /2 - SNAKE_SIZE /2;
    if (startingPosition % 2 == 1)
        startingPosition -= 1;

    snake->tail.pos.x = startingPosition;
    snake->head.pos.x = startingPosition + SNAKE_SIZE - SEGMENT_SIZE;
    snake->tail.pos.y = snake->head.pos.y = window.rows /2 + TOP_BORDER_OFFSET;
}

void move_snake(Snake *snake, Food *food, Timer *timer, Stats *stats)
{   
    int speedMod[] = {SLOW, FAST};
    enum lifeMod {subtract, add};
    Eats *eats = NULL;

    HANDLE hstdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdIn == INVALID_HANDLE_VALUE)
        printf("Error getting input handle.");

    // flush any remaining input events from the buffer
    FlushConsoleInputBuffer(hstdIn);
  
    start_timer(timer->begin);

    while (1) 
    {
        int growSnake = 0;

        timer->elapsed = stop_timer(timer->end) - timer->begin;

        int newDir = get_next_move(snake->dir, stats);

        // adjust timer if the menu was open
        timer->begin += stop_timer(timer->end) - timer->begin - timer->elapsed;

        check_modifier(snake, timer, stats);
        check_food(snake, food, timer, stats);
        change_head_position(snake, newDir);

        if (is_border(snake) || (snake->segCount >= 3 && is_snake(snake, snake->head.pos, CHECK_SNAKE)))
        {   
            if (stats->lives)
                update_lives(stats, subtract);
            if (stats->speedActive || stats->lifeActive)
                hide_modifier();
            break;
        }

        if ((eats = is_food(food, snake->head.pos)) != NULL)
        {
            update_score(stats, eats);
            if (stats->score > stats->hiscore)
                update_hiscore(stats);

            growSnake = 1;
            
            if (eats->ftype == hidden)
            {       
                if (eats->mod == slow || eats->mod == fast)
                {
                    snake->speed = speedMod[eats->mod];
                    stats->speedActive = 1;
                    timer->resetSpeed = timer->elapsed;
                }               
                else if (eats->mod == life)
                {
                    update_lives(stats, add);
                    stats->lifeActive = 1;
                    timer->resetLife = timer->elapsed;
                }
                display_modifier(eats);
            }
        }
        if (!growSnake)
            change_tail_position(snake);

        display_new_position(snake, growSnake);
        Sleep(snake->speed);
    }

    Sleep(1000);
    clear_board();
    free(snake->seg);
}

// check if we need to hide the modifiers
void check_modifier(Snake *snake, Timer *timer, Stats *stats)
{
    if (stats->speedActive && timer->elapsed - timer->resetSpeed > SPEED_DELAY)
    {
        snake->speed = NORMAL;
        stats->speedActive = 0;
        hide_modifier();
    }
    else if (stats->lifeActive && timer->elapsed - timer->resetLife > LIFE_DELAY)
    {
        stats->lifeActive = 0;
        hide_modifier();
    }
}

// check for console input events
Direction get_next_move(Direction dir, Stats *stats)
{
    DWORD events = 0, eventsRead = 0;
    INPUT_RECORD buff[MAX_EVENTS];

    HANDLE hstdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdIn == INVALID_HANDLE_VALUE)
        printf("Error getting input handle.");
        
    if(!GetNumberOfConsoleInputEvents(hstdIn, &events))
        printf("Error getting number of console input events.");

    if (events)
    {
        if (!ReadConsoleInput(hstdIn, buff, MAX_EVENTS, &eventsRead))
            printf("Error reading console input.");

        for (int i = 0; i < eventsRead; i++)
        {
            if (buff[i].EventType == KEY_EVENT && buff[i].Event.KeyEvent.bKeyDown)
            {
                switch (buff[i].Event.KeyEvent.wVirtualKeyCode)
                {
                    case VK_LEFT:
                        if (dir != right) return left;
                        break;
                    case VK_RIGHT:
                        if (dir != left) return right;
                        break;
                    case VK_UP:
                        if (dir != down) return up;
                        break;
                    case VK_DOWN:
                        if (dir != up) return down;
                        break;
                    case VK_ESCAPE:
                        if (stats->hiscore > get_hiscore())
                            save_hiscore(stats);
                        display_menu(IN_GAME_MENU);
                        break;
                    case VK_SPACE:
                        if (stats->hiscore > get_hiscore())
                            save_hiscore(stats);
                        game_paused();
                        break;
                }
            }
        }
    }  
    return dir;
}

void game_paused(void)
{
    char *gamePaused[] = {"Game paused", "Press [space] to continue"};
    DWORD eventsRead = 0;
    INPUT_RECORD buff;

    enable_alt_screen();
    clear_screen();
    clear_scrollback();
    hide_cursor();

    HANDLE hstdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdIn == INVALID_HANDLE_VALUE)
        printf("Error getting input handle.");

    Window window = get_window_size();

    move_cursor(window.cols /2 - (int) strlen(gamePaused[0]) /2, window.rows /2);
    printf("%s", gamePaused[0]);
    move_cursor(window.cols /2 - (int) strlen(gamePaused[1]) /2, window.rows /2 + 2);
    printf("%s", gamePaused[1]);

    while (1)
    {
        wait_for_key_press(hstdIn);

        if (!ReadConsoleInput(hstdIn, &buff, 1, &eventsRead))
            printf("Error reading console input.");

        if (buff.EventType == KEY_EVENT && buff.Event.KeyEvent.bKeyDown)
        {
            if (buff.Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
            {
                disable_alt_screen();
                return;
            }
        }
        else if (buff.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
        {
            disable_alt_screen();
            clear_screen();
            clear_scrollback();
            show_cursor();
            exit(EXIT_SUCCESS);
        }
    }
}

// set new head position
void change_head_position(Snake *snake, int newDir)
{
    // if there is a change in direction, we increase the segment count 
    if (snake->dir != newDir)
    {
        if (snake->allocatedMemory - snake->segCount == 1)
            allocate_memory(snake, snake->allocatedMemory + BLOCK_SIZE);
                
        snake->segCount++;
        snake->seg[snake->segCount-1] = snake->head.pos;
    }   

    snake->dir = newDir;
    snake->head.oldPos = snake->head.pos;

    switch (snake->dir)
    {
        case left:
            snake->head.pos.x -= 2;
            break;
        case right:
            snake->head.pos.x += 2;
            break;
        case up:
            snake->head.pos.y -= 1;
            break;
        case down:
            snake->head.pos.y += 1;
            break;
    }
}

// set new tail position
void change_tail_position(Snake *snake)
{
    int dir;

    if (snake->segCount)
        dir = set_tail_direction(snake);
    else 
        dir = snake->dir;
    
    snake->tail.oldPos = snake->tail.pos;
    
    switch (dir)
    {
        case left:
            snake->tail.pos.x -= 2;
            break;
        case right:
            snake->tail.pos.x += 2;
            break;
        case up:
            snake->tail.pos.y -= 1;
            break;
        case down:
            snake->tail.pos.y += 1;
            break;
    }
    if (snake->segCount && is_same_position(snake->tail.pos, snake->seg[0]))
        remove_segment(snake);
}

 // set tail direction according to the last segment position
int set_tail_direction(Snake *snake)
{
    int dir;

    if (snake->seg[0].x == snake->tail.pos.x)
        dir = snake->seg[0].y < snake->tail.pos.y ? up : down;

    else if (snake->seg[0].y == snake->tail.pos.y)
        dir = snake->seg[0].x > snake->tail.pos.x ? right : left;

    return dir;
}

// remove last segment if the tail is at its position
void remove_segment(Snake *snake)
{
     for (int i = 0; i < snake->segCount - 1; i++)
        snake->seg[i] = snake->seg[i+1];
    snake->segCount--;
}

// A single snake segment consists of two characters
void display_starting_position(Snake *snake)
{
    move_cursor(snake->tail.pos.x, snake->tail.pos.y);

    for (int i = 0; i < SNAKE_SIZE /2 - 1; i++)
        printf("%s%s%s", CYAN, SNAKE_SEG, RESET_COLOR);
    printf("%s%s%s", BRIGHT_CYAN, SNAKE_SEG, RESET_COLOR);
}

// display next position
void display_new_position(Snake *snake, int growSnake)
{   
    move_cursor(snake->head.pos.x, snake->head.pos.y);
    printf("%s%s%s", BRIGHT_CYAN, SNAKE_SEG, RESET_COLOR);

    move_cursor(snake->head.oldPos.x, snake->head.oldPos.y);
    printf("%s%s%s", CYAN, SNAKE_SEG, RESET_COLOR);

    if (!growSnake)
    {
        move_cursor(snake->tail.oldPos.x, snake->tail.oldPos.y);
        printf("%s%s", BLANK, RESET_COLOR);
    }
}

// check if snake will collide with the border
int is_border(Snake *snake)
{
    Border border = get_border_coordinates();

    return snake->head.pos.x + 1 == border.left || snake->head.pos.x == border.right || snake->head.pos.y == border.top || snake->head.pos.y == border.bottom;
}

// check if snake will collide with itself
int is_snake(Snake *snake, Position obj, int objectType)
{
    Position seg;
    
    if (!snake->segCount && is_intersection(snake->head.pos, snake->tail.pos, obj))
        return 1;
    else 
    {
        for (int i = 0; i < snake->segCount; i++)
        {
            seg = i == 0 ? snake->tail.pos : snake->seg[i-1];
            if (is_intersection(seg, snake->seg[i], obj))
                return 1;
        }
        if (objectType && is_intersection(snake->seg[snake->segCount-1], snake->head.pos, obj))
            return 1;
    }
    return 0;
}

// check if there is food ahead
Eats * is_food(Food *food, Position obj)
{
    if (food->stdCount)
    {
        for (int i = 0; i < food->maxFood; i++)
        {
            if (is_same_position(food->standard[i].pos, obj))
            {   
                food->standard[i].pos.x = food->standard[i].pos.y = 0;
                food->stdCount--;
                return &food->standard[i]; 
            }
        }
    }
    if (food->valCount && is_same_position(food->value.pos, obj))
    {
        food->valCount = 0;
        return &food->value;
    }
    if (food->hidCount && is_same_position(food->hidden.pos, obj))
    {
        food->hidCount = 0;
        return &food->hidden;
    }
    return NULL;
}

int is_same_position(Position p1, Position p2)
{
    return (p1.x == p2.x && p1.y == p2.y);
}

// check if snake's head or food intersects with the snake
int is_intersection(Position seg1, Position seg2, Position obj)
{
    if (seg1.x == seg2.x && seg1.x == obj.x)
        return (seg1.y >= obj.y && seg2.y <= obj.y) || (seg1.y <= obj.y && seg2.y >= obj.y);
    else if (seg1.y == seg2.y && seg1.y == obj.y)
        return (seg1.x >= obj.x && seg2.x <= obj.x) || (seg1.x <= obj.x && seg2.x >= obj.x);
    
    return 0;
}

// clear board
void clear_board(void)
{
    Border border = get_border_coordinates();

    move_cursor(border.left + 1, border.top + 1);

    for (int i = border.top + 1; i < border.bottom; i++)
    {
        erase_characters(border.right - border.left - 1);
        move_cursor_down(1);
    }
}

// display game over message
void display_game_over_msg(void)
{
    char *gameOverMsg = "GAME OVER";

    Window window = get_window_size();

    move_cursor(window.cols /2 - (int) strlen(gameOverMsg) /2, window.rows /2 + TOP_BORDER_OFFSET);
    printf("%s%s%s", BRIGHT_RED, gameOverMsg, RESET_COLOR);
    Sleep(3000);
}