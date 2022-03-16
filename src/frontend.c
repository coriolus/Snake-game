#define _CRT_SECURE_NO_WARNINGS
#include "frontend.h"
#include "backend.h"
#include "menu.h"
#include "stats.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define LOGO_HEIGHT 5
#define LOGO_LENGTH 35
#define MAX_LENGTH 100
#define MIN_COLS 80
#define MIN_ROWS 25

#define WINDOW_SIZE_MSG1 "Minimum window size: " STR(MIN_COLS) " cols and " STR(MIN_ROWS) " rows."
#define WINDOW_SIZE_MSG2 "Please resize the console window and restart the game."
#define STR(X) STR2(X)
#define STR2(X) #X

int enable_vt_mode(void);
int min_window_size(void);
void load_intro(void);
void display_logo(char **, int, int);
void display_backup_logo(void);
void draw_borders(void);
void draw_horizontal_border(Border, int);
void draw_vertical_border(Border, int);

void set_up_console(int firstRun)
{
    if (!enable_vt_mode() || !min_window_size())
        exit(EXIT_FAILURE);

    if (firstRun)
        load_intro();

    display_menu(START_MENU);
    draw_borders();
    display_stats();
}

// enable ANSI escape sequences in console
int enable_vt_mode(void)
{
    DWORD mode;

    HANDLE hstdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hstdOut == INVALID_HANDLE_VALUE)
    {
        printf("Error getting output handle.");
        return 0;
    }
    if (!GetConsoleMode(hstdOut, &mode))
    {
        printf("Error getting console mode.");
        return 0;
    }

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hstdOut, mode))
    {
        printf("Error setting console mode.");
        return 0;
    }

    return 1;
}

// check minimum window size
int min_window_size(void)
{
    Window window = get_window_size();

    if (window.cols < MIN_COLS || window.rows < MIN_ROWS)
    {
        enable_alt_screen();
        clear_screen();
        clear_scrollback();
        hide_cursor();

        // display warning message if the size is too small and exit the program
        move_cursor(window.cols /2 - (int) strlen(WINDOW_SIZE_MSG1) /2, window.rows /2);
        printf("%s\n", WINDOW_SIZE_MSG1);
        move_cursor(window.cols /2 - (int) strlen(WINDOW_SIZE_MSG2) /2, window.rows /2 + 1);
        printf("%s", WINDOW_SIZE_MSG2);
        Sleep(5000);

        disable_alt_screen();
        clear_screen();
        clear_scrollback();
        show_cursor();

        return 0;
    }
    return 1;
}

// load intro screen
void load_intro(void)
{
    int ch, lines = 0, lineLength = 0;
    char buff[MAX_LENGTH] = {0};

    FILE *fp = fopen("snake.txt", "r");

    // display backup logo if the file can't be opened
    if (fp == NULL)
        display_backup_logo();
    else
    {
        // count lines to determine logo height
        while ((ch = fgetc(fp)) != EOF)
        {
            if (ch == '\n')
                lines++;
        }
        rewind(fp);

        char **arr = (char **) malloc(lines * sizeof(char*));
        if (arr == NULL)
        {
            printf("Memory not allocated.");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < lines; i++)
        {
            fgets(buff, MAX_LENGTH, fp);
            if (buff[strlen(buff)-1] == '\n')
                buff[strlen(buff)-1] = '\0';
            
            // allocate memory for each string and copy strings from the buffer
            arr[i] = (char *) calloc(strlen(buff) + 1, sizeof(char));
            if (arr[i] == NULL)
            {
                printf("Memory not allocated.");
                exit(EXIT_FAILURE);
            }
            strcpy(arr[i], buff);

            // determine logo width
            if (strlen(buff) > lineLength)
                lineLength = strlen(buff);
        }   
        fclose(fp);

        // display main logo
        display_logo(arr, lines, lineLength);

        for (int i = 0; i < lines; i++)
        {
            free(arr[i]);
        }
        free(arr);
    }
}

void display_logo(char **arr, int lines, int lineLength)
{
    // activate alternate screen buffer
    enable_alt_screen();
    clear_screen();
    clear_scrollback();
    hide_cursor();

    // the size of the window (rows x cols) must be >= than lines x lineLength
    Window window = get_window_size();

    // move cursor to the center of the window
    move_cursor(window.cols /2 - lineLength/ 2, window.rows /2 - lines/ 2);
    save_cursor_position();

    for (int i = 0; i < lines; i++)
    {
        set_color(BRIGHT_BLUE);
        printf("%s%s", arr[i], RESET_COLOR);
        restore_cursor_position();
        move_cursor_down(i+1);
    }
    Sleep(5000);
    reset_color();
    clear_screen();
    clear_scrollback();
    disable_alt_screen();
    show_cursor();
}

void display_backup_logo(void)
{
    char logo[LOGO_HEIGHT][LOGO_LENGTH] = {"##### #    #    #    #    # #####",
                                           "#     ##   #   # #   #   #  #", 
                                           "##### # #  #  ## ##  ###    ####",
                                           "    # #   ## #     # #   #  #",
                                           "##### #    # #     # #    # #####"};
    
    // activate alternate screen buffer
    enable_alt_screen();
    clear_screen();
    clear_scrollback();
    hide_cursor();

    Window window = get_window_size();

    // move cursor to the center of the window
    move_cursor(window.cols /2 - LOGO_LENGTH/ 2, window.rows /2 - LOGO_HEIGHT /2);
    save_cursor_position();

    for (int i = 0; i < LOGO_HEIGHT; i++)
    {   
        if (i < LOGO_HEIGHT - 1)
            set_color(BRIGHT_BLUE);
        else
            set_color(BLUE);
        printf("%s%s", logo[i], RESET_COLOR);
        restore_cursor_position();
        move_cursor_down(i+1);
    }
    Sleep(5000);
    reset_color();
    clear_screen();
    clear_scrollback();
    disable_alt_screen();
    show_cursor();
}

// create rectangular bordered area
void draw_borders(void)
{     
    clear_screen();
    clear_scrollback();

    Border border = get_border_coordinates();

    // top border
    move_cursor(border.left, border.top);
    draw_horizontal_border(border, 1);

    // bottom border
    move_cursor(border.left, border.bottom);
    draw_horizontal_border(border, 0);

    // left border
    move_cursor(border.left, border.top + 1);
    draw_vertical_border(border, 1);

    // right border
    move_cursor(border.right, border.top + 1);
    draw_vertical_border(border, 0);
}

void draw_horizontal_border(Border border, int isTop)
{
    enable_line_drawing_mode();
    set_color(MAGENTA);

    // print corners
    putchar(isTop ? TOP_LEFT_CORNER : BOTTOM_LEFT_CORNER);

    // print horizontal lines
    for (int i = 0; i < border.right - 2; i++)
        printf("%c", HORIZONTAL_BAR);

    putchar(isTop ? TOP_RIGHT_CORNER : BOTTOM_RIGHT_CORNER);

    reset_color();
    disable_line_drawing_mode();
}

void draw_vertical_border(Border border, int isLeft)
{
    enable_line_drawing_mode();
    set_color(MAGENTA);

    // print vertical lines
    for (int i = 0; i < border.bottom - TOP_BORDER_OFFSET - 2; i++)
    {
        save_cursor_position();
        printf("%c", VERTICAL_BAR);
        restore_cursor_position();
        move_cursor_down(1);
    }

    reset_color();
    disable_line_drawing_mode();
}

// get console window size
Window get_window_size(void)
{
    Window window;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    HANDLE hstdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hstdOut == INVALID_HANDLE_VALUE)
        printf("Error getting output handle.");

    if (!GetConsoleScreenBufferInfo(hstdOut, &csbi))
        printf("Error getting console screen buffer info.");

    window.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    window.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    return window;
}

// get border coordinates
Border get_border_coordinates(void)
{
    Border border;

    Window window = get_window_size();

    /* If the number of columns is odd,
    adjust position of the vertical border */
    int adjustCols = window.cols % 2 == 0 ? 0 : 1;

    border.left = 1;
    border.right = window.cols - adjustCols;
    border.top = 1 + TOP_BORDER_OFFSET;
    border.bottom = window.rows;

    return border;
}
