#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef enum {left, right, up, down} Direction;
typedef enum {standard, value, hidden} FoodType;
typedef enum {slow, fast, life} Modifier;
typedef enum {stdStart, stdEnd, valStart, valEnd, hidStart, hidEnd} DelayValues;
typedef enum {hiscore, lives, score} StatsValues;
typedef enum {startGame, settings, exitGame, resumeGame} Menu;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    Position oldPos;
} EndPos;

typedef struct {
    EndPos head;
    EndPos tail;
    Position *seg;
    int segCount;
    int allocatedMemory;
    int speed;
    Direction dir;
} Snake;

typedef struct {
    Position pos;
    time_t time;
    Modifier mod;
    FoodType ftype;
} Eats;

typedef struct {
    Eats standard[15];
    Eats value;
    Eats hidden;
    int stdCount, valCount, hidCount;
    int foodToAdd;
    int maxFood;
} Food;

typedef struct {
    time_t begin, end;
    time_t elapsed;
    time_t resetStd, resetVal, resetHid, resetSpeed, resetLife;
    int delay[6];
} Timer;

typedef struct {
    int lives;
    int score;
    int hiscore;
    int lifeAdded;
    int lifeActive;
    int speedActive;
} Stats;

typedef struct {
    int rows;
    int cols;
} Window;

typedef struct {
    int left;
    int right;
    int top;
    int bottom;
} Border;

#endif /* TYPES_H */