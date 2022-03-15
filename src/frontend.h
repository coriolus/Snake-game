#ifndef FRONTEND_H
#define FRONTEND_H

#include "types.h"

void set_up_console(int);
Window get_window_size(void);
Border get_border_coordinates(void);

// color modes
#define BOLD "\x1B[1m"
#define DIM "\x1B[2m"
#define BLINK "\x1B[5m"
#define INVERSE "\x1B[7m"
#define RESET_COLOR "\x1B[0m"

// foreground colors
#define BLACK "\x1B[30m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"

// bright foreground colors
#define BRIGHT_BLACK "\x1B[90m"
#define BRIGHT_RED "\x1B[91m"
#define BRIGHT_GREEN "\x1B[92m"
#define BRIGHT_YELLOW "\x1B[93m"
#define BRIGHT_BLUE "\x1B[94m"
#define BRIGHT_MAGENTA "\x1B[95m"
#define BRIGHT_CYAN "\x1B[96m"
#define BRIGHT_WHITE "\x1B[97m"

// background colors
#define BG_BLACK "\x1B[40m"
#define BG_RED "\x1B[41m"
#define BG_GREEN "\x1B[42m"
#define BG_YELLOW "\x1B[43m"
#define BG_BLUE "\x1B[44m"
#define BG_MAGENTA "\x1B[45m"
#define BG_CYAN "\x1B[46m"
#define BG_WHITE "\x1B[47m"

#define set_color(X) printf("%s", X)
#define reset_color() printf("\x1B[0m")

// clear screen and erase characters
#define clear_screen() printf("\x1B[H\x1B[2J")
#define clear_scrollback() printf("\x1B[3J")
#define erase_characters(X) printf("\x1B[%dX", X)

// cursor control
#define hide_cursor() printf("\x1B[?25l")
#define show_cursor() printf("\x1B[?25h")

#define move_cursor(X,Y) printf("\x1B[%d;%dH", Y, X)
#define move_cursor_left(X) printf("\x1B[%dD", X)
#define move_cursor_right(X) printf("\x1B[%dC", X)
#define move_cursor_up(X) printf("\x1B[%dA", X)
#define move_cursor_down(X) printf("\x1B[%dB", X)

// cursor position
#define save_cursor_position() printf("\x1B[s")
#define restore_cursor_position() printf("\x1B[u")
#define report_cursor_position() printf("\x1B[6n")

// tab stops
#define clear_tab_stops() printf("\x1B[3g")
#define set_tab_stop() printf("\x1BH")

// line drawing mode
#define enable_line_drawing_mode() printf("\x1B(0")
#define disable_line_drawing_mode() printf("\x1B(B")

// alternate screen buffer
#define enable_alt_screen() printf("\x1B[?1049h")
#define disable_alt_screen() printf("\x1B[?1049l")

// border characters
#define VERTICAL_BAR 'X'
#define HORIZONTAL_BAR 'X'
#define TOP_LEFT_CORNER '\x6C'
#define TOP_RIGHT_CORNER '\x6B'
#define BOTTOM_LEFT_CORNER '\x6D'
#define BOTTOM_RIGHT_CORNER '\x6A'

// display and erase snake segments
#define SNAKE_SEG "[]"
#define BLANK "  "

#endif /* FRONTEND_H */