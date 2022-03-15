#define _CRT_SECURE_NO_WARNINGS
#include "menu.h"
#include "frontend.h"
#include "stats.h"
#include "types.h"

#include <stdio.h>
#include <string.h>
#include <windows.h>

void display_menu(int menuType)
{
    char *menu[] = {"Start game", "Settings", "Exit game", "Resume game"};
    int x, y;
    Menu selected = startGame;
    DWORD eventsRead = 0;
    INPUT_RECORD buff;

    int menuItems = sizeof(menu) / sizeof(*menu) - 1;

    if (menuType == IN_GAME_MENU)
    {
        char *temp = menu[startGame];
        menu[startGame] = menu[resumeGame];
        menu[resumeGame] = temp;
    }

    enable_alt_screen();
    hide_cursor();

    HANDLE hstdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdIn == INVALID_HANDLE_VALUE)
        printf("Error getting input handle.");

    Window window = get_window_size();

    // move cursor to the center of the window
    move_cursor(window.cols /2 - (int) strlen(menu[startGame]) /2, window.rows /2 - menuItems /2);
    save_cursor_position();

    // display menu items
    for (int i = 0; i < menuItems; i++)
    {
        if (i == 0)
            printf("%s%s%s", INVERSE, menu[i], RESET_COLOR);
        else 
            printf("%s", menu[i]);
        restore_cursor_position();
        move_cursor_down(i+1);
    }
    restore_cursor_position();
    get_cursor_position(&x, &y);
    
    FlushConsoleInputBuffer(hstdIn);

    // check for key press and, if present, select menu item
    while (1)
    {
        wait_for_key_press(hstdIn);

        if (!ReadConsoleInput(hstdIn, &buff, 1, &eventsRead))
            printf("Error reading console input.");

        if (buff.EventType == KEY_EVENT && buff.Event.KeyEvent.bKeyDown)
        {
            switch (buff.Event.KeyEvent.wVirtualKeyCode)
            {
                case VK_UP:
                    if (selected == startGame)
                    {
                        deselect_menu_item(x, y, startGame, menu);
                        select_menu_item(x, y+2, exitGame, menu);
                        selected = exitGame;
                    }   
                    else if (selected == settings)
                    {
                        deselect_menu_item(x, y+1, settings, menu);
                        select_menu_item(x, y, startGame, menu);
                        selected = startGame;
                    }
                    else if (selected == exitGame)
                    {
                        deselect_menu_item(x, y+2, exitGame, menu);
                        select_menu_item(x, y+1, settings, menu);
                        selected = settings;
                    }
                    break;
                case VK_DOWN:
                    if (selected == startGame)
                    {
                        deselect_menu_item(x, y, startGame, menu);
                        select_menu_item(x, y+1, settings, menu);
                        selected = settings;
                    } 
                    else if (selected == settings)
                    {
                        deselect_menu_item(x, y+1, settings, menu);
                        select_menu_item(x, y+2, exitGame, menu);
                        selected = exitGame;
                    }
                    else if (selected == exitGame)
                    {
                        deselect_menu_item(x, y+2, exitGame, menu);
                        select_menu_item(x, y, startGame, menu);
                        selected = startGame;
                    }
                    break;
                case VK_RETURN:
                    if (selected == startGame)
                    {
                        disable_alt_screen();
                        return;
                    }
                    else if (selected == exitGame)
                    {
                        disable_alt_screen();
                        clear_screen();
                        clear_scrollback();
                        show_cursor();
                        exit(EXIT_SUCCESS);
                    }
            }
        }
    }
}

// wait until console input is in the signaled state 
void wait_for_key_press(HANDLE hstdIn)
{
    DWORD oldMode, mode;

    if (!GetConsoleMode(hstdIn, &oldMode))
        printf("Error getting console mode.");

    mode = oldMode;
    mode &= ~(ENABLE_MOUSE_INPUT | ENABLE_QUICK_EDIT_MODE);
    if (!SetConsoleMode(hstdIn, mode))
        printf("Error setting console mode.");

    WaitForSingleObject(hstdIn, INFINITE);
    
    if (!SetConsoleMode(hstdIn, oldMode))
        printf("Error setting console mode.");
}

// get cursor position for the menu
void get_cursor_position(int *x, int *y)
{
    DWORD oldMode, mode;

    HANDLE hstdIn = GetStdHandle(STD_INPUT_HANDLE);
    if (hstdIn == INVALID_HANDLE_VALUE)
        printf("Error getting input handle.");
        
    if (!GetConsoleMode(hstdIn, &oldMode))
        printf("Error getting console mode.");

    mode = oldMode;
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    if (!SetConsoleMode(hstdIn, mode))
        printf("Error setting console mode.");

    report_cursor_position();
    scanf("\x1B[%d;%dR", y, x);

    if (!SetConsoleMode(hstdIn, oldMode))
        printf("Error setting console mode.");
}

// select menu item
void select_menu_item(int x, int y, Menu selected, char *menu[])
{
    move_cursor(x, y);
    printf("%s%s%s", INVERSE, menu[selected], RESET_COLOR);
}

// deselect menu item
void deselect_menu_item(int x, int y, Menu selected, char *menu[])
{
    move_cursor(x, y);
    printf("%s", menu[selected]);
}