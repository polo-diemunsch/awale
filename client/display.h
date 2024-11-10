#ifndef DISPLAY_H
#define DISPLAY_H

#include <sys/ioctl.h>

#include "../communication/communication.h"

// TODO define somewhere else ?
#define BOARD_SIZE           12

#define VERTICAL_PADDING        1
#define HORIZONTAL_PADDING      (2 * VERTICAL_PADDING)

#define TITLE_HEIGHT            (1 + VERTICAL_PADDING)

#define MENU_AND_GAME_WIDTH     (69 + 2 * HORIZONTAL_PADDING)
#define MENU_AND_GAME_HEIGHT    (13 + 2 * VERTICAL_PADDING)

#define BOARD_WIDTH             ((BOARD_SIZE / 2) * 5 + 1)

#define CHAT_WIDTH              (MENU_AND_GAME_WIDTH + 2)

#define MIN_WIDTH               CHAT_WIDTH
#define MIN_HEIGHT              (TITLE_HEIGHT + 2 * MENU_AND_GAME_HEIGHT - VERTICAL_PADDING)

typedef struct {
    Message messages[BUF_SIZE];
    int oldest_message_index;
    int newest_message_index;
    int oldest_displayed_message_index;
    int newest_displayed_message_index;
} Messages;

void update_interface(struct winsize ws, Messages *messages);

void display_window_too_small(struct winsize ws);

typedef struct {
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
} Position;

void display_title(Position position);
void display_menu(Position position);
void display_game(Position position);
void display_chat(Position position, Messages *messages);

char *truncate_name(char *destination, char *source, size_t n);

#endif /* guard */
