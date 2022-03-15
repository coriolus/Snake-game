#ifndef GAME_MENU_H
#define GAME_MENU_H

#include "types.h"
#include <windows.h>

#define START_MENU 0
#define IN_GAME_MENU 1

void display_menu(int);
void wait_for_key_press(HANDLE);
void get_cursor_position(int *, int *);
void select_menu_item(int, int, Menu, char *[]);
void deselect_menu_item(int, int, Menu, char *[]);

#endif /* GAME_MENU_H */