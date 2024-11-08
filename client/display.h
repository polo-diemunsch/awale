#ifndef DISPLAY_H
#define DISPLAY_H

#include <sys/ioctl.h>

// TODO define somewhere else ?
#define BOARD_SIZE 12

#define VERTICAL_MARGIN 1
#define HORIZONTAL_MARGIN (2 * VERTICAL_MARGIN)
#define MAX_CHAT_WIDTH 100
#define GAME_WIDTH ((BOARD_SIZE / 2) * 5 + 1)
#define GAME_HEIGHT 9
#define MIN_HEIGHT (GAME_HEIGHT + 2 * HORIZONTAL_MARGIN)
#define MAX_HEIGHT 25
#define MIN_FULLSIZE_WIDTH (2 * GAME_WIDTH + 3 * HORIZONTAL_MARGIN)
#define MIN_HALFSIZE_WIDTH (GAME_WIDTH + 2 * HORIZONTAL_MARGIN)
#define MIN_HALFSIZE_HEIGHT (2 * GAME_HEIGHT + 3 * VERTICAL_MARGIN)

#define DISPLAYED_NAME_MAX_SIZE 26

#define clear_screen() printf("\033[1;1H\033[2J")
#define move_cursor_to(x,y) printf("\033[%d;%dH", (y), (x))
#define move_cursor_up(x) printf("\033[%dA", (x))
#define move_cursor_down(x) printf("\033[%dB", (x))
#define move_cursor_right(x) printf("\033[%dC", (x))
#define move_cursor_left(x) printf("\033[%dD", (x))

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

void update_interface(struct winsize ws);
void display_fullsize(struct winsize ws);
void display_halfsize(struct winsize ws);
void display_window_too_small(struct winsize ws);

typedef struct {
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
} Position;

void display_chat(Position position);
void display_game(Position position);

char * truncate_name(char * destination, char * source, size_t n);

#endif /* guard */
