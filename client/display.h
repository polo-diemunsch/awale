#ifndef DISPLAY_H
#define DISPLAY_H

#include <sys/ioctl.h>

#include "../serialization/serialization.h"

#define VERTICAL_PADDING     1
#define HORIZONTAL_PADDING   (2 * VERTICAL_PADDING)

#define TITLE_HEIGHT         (1 + VERTICAL_PADDING)

#define MENU_AND_GAME_WIDTH  (69 + 2 * HORIZONTAL_PADDING)
#define MENU_AND_GAME_HEIGHT (14 + 2 * VERTICAL_PADDING)

#define BOARD_WIDTH          ((BOARD_SIZE / 2) * 5 + 1)

#define CHAT_WIDTH           (MENU_AND_GAME_WIDTH + 2)

#define MIN_WIDTH            CHAT_WIDTH
#define MIN_HEIGHT           (TITLE_HEIGHT + 2 * MENU_AND_GAME_HEIGHT - VERTICAL_PADDING)

#define MESSAGES_COUNT       128

typedef struct {
    char *messages[MESSAGES_COUNT];
    int oldest_message_index;
    int newest_message_index;
    int oldest_displayed_message_index;
    int newest_displayed_message_index;
} Messages;

void update_interface(struct winsize ws, Messages *messages, Game *game, char who_am_i);

void display_window_too_small(struct winsize ws);

typedef struct {
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
} Position;

void display_title(Position position);
void display_menu(Position position);
void display_game(Position position, Game *game, unsigned char who_am_i);
void display_chat(Position position, Messages *messages);

int get_visual_length(const char *string);
int get_string_length_for_visual_length(const char *string, int visual_length);

char *truncate_name(char *destination, const char *source, size_t n);

void multiline_print(const char *string, unsigned short first_column, unsigned short content_length);

#endif /* guard */
