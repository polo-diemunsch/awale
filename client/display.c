#include <stdio.h>
#include <string.h>
#include "display.h"

void update_interface(struct winsize ws) {
    clear_screen();

    if (ws.ws_col < MIN_FULLSIZE_WIDTH)
        display_halfsize(ws);
    else
        display_fullsize(ws);
}

void display_fullsize(struct winsize ws) {
    if (ws.ws_col < MIN_FULLSIZE_WIDTH || ws.ws_row < MIN_HEIGHT) {
        display_window_too_small(ws);
        return;
    }

    unsigned short height = min(max(ws.ws_row, MIN_HEIGHT), MAX_HEIGHT);

    unsigned short chat_width = ws.ws_col - GAME_WIDTH - 3 * HORIZONTAL_MARGIN;

    Position game_position = {
        .x = 1 + chat_width + 2 * HORIZONTAL_MARGIN,
        .y = 1 + (height - GAME_HEIGHT) / 2,
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT,
    };
    display_game(game_position);

    Position chat_position = {
        .x = 1 + HORIZONTAL_MARGIN,
        .y = 1 + VERTICAL_MARGIN,
        .width = chat_width,
        .height = height - 2 * VERTICAL_MARGIN,
    };
    display_chat(chat_position);
}

void display_halfsize(struct winsize ws) {
    if (ws.ws_col < MIN_HALFSIZE_WIDTH || ws.ws_row < MIN_HALFSIZE_HEIGHT) {
        display_window_too_small(ws);
        return;
    }

    Position game_position = {
        .x = (ws.ws_col - GAME_WIDTH) / 2,
        .y = 1 + VERTICAL_MARGIN,
        .width = GAME_WIDTH,
        .height = GAME_HEIGHT,
    };
    display_game(game_position);

    Position chat_position = {
        .x = 1 + HORIZONTAL_MARGIN,
        .y = 1 + GAME_HEIGHT + 2 * VERTICAL_MARGIN,
        .width = ws.ws_col - 2 * HORIZONTAL_MARGIN,
        .height = ws.ws_row - GAME_HEIGHT - 3 * VERTICAL_MARGIN,
    };
    display_chat(chat_position);
}

void display_window_too_small(struct winsize ws) {
    move_cursor_to(1 + max(0, ws.ws_col - 35) / 2, 1 + (ws.ws_row - 1) / 2);
    printf("Window too small, please resize it\n");
}

void display_chat(Position position) {
    move_cursor_to(position.x, position.y);
    printf("╭");
    for (int i = 0; i < position.width - 2; i++) printf("─");
    printf("╮");

    for (int i = 1; i < position.height - 1; i++) {
        move_cursor_to(position.x, position.y + i);
        printf("│");
        move_cursor_to(position.x + position.width - 1, position.y + i);
        printf("│");
    }

    move_cursor_to(position.x, position.y + position.height - 1);
    printf("╰");
    for (int i = 0; i < position.width - 2; i++) printf("─");
    printf("╯");

    move_cursor_to(position.x + 1, position.y + position.height - 2);
    printf("> ");

    fflush(stdout);
}

void display_game(Position position) {
    int board[BOARD_SIZE] = {1, 2, 4, 6, 8, 12, 3, 0, 6, 8, 2, 3};
    char name_player2[] = "Joueur adverse";
    unsigned int score_player2 = 0;
    char name_player1[] = "Mais wtf il est super long ton pseudo";
    unsigned int score_player1 = 69;
    char displayed_name[DISPLAYED_NAME_MAX_SIZE + 1];

    move_cursor_to(position.x, position.y);
    printf(" %-26s%2d", truncate_name(displayed_name, name_player2, DISPLAYED_NAME_MAX_SIZE), score_player2);

    move_cursor_to(position.x, position.y + 2);
    printf("╭────┬────┬────┬────┬────┬────╮");

    move_cursor_to(position.x, position.y + 3);
    for (int i = BOARD_SIZE - 1; i >= BOARD_SIZE / 2; i--)
        printf("│ %2d ", board[i]);
    printf("│");

    move_cursor_to(position.x, position.y + 4);
    printf("├────┼────┼────┼────┼────┼────┤");

    move_cursor_to(position.x, position.y + 5);
    for (int i = 0; i < BOARD_SIZE / 2; i++)
        printf("│ %2d ", board[i]);
    printf("│");

    move_cursor_to(position.x, position.y + 6);
    printf("╰────┴────┴────┴────┴────┴────╯");

    move_cursor_to(position.x, position.y + 8);
    printf(" %-26s%2d", truncate_name(displayed_name, name_player1, DISPLAYED_NAME_MAX_SIZE), score_player1);

    fflush(stdout);
}

char * truncate_name(char * destination, char * source, size_t n) {
    strncpy(destination, source, n);

    if (strlen(source) > n) {
        strcpy(destination + n - 4, "...");
        destination[n] = '\0';
    }

    return destination;
}
