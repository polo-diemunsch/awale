#include <stdio.h>
#include <string.h>

#include "display.h"
#include "../lib/console.h"
#include "../lib/math.h"

void update_interface(struct winsize ws, Messages *messages)
{
    clear_screen();

    if (ws.ws_col < MIN_WIDTH || ws.ws_row < MIN_HEIGHT)
    {
        display_window_too_small(ws);
        return;
    }

    Position title_position = {
        .x = 1,
        .y = 1,
        .width = ws.ws_col,
        .height = TITLE_HEIGHT,
    };
    display_title(title_position);

    Position menu_game_position = {
        .x = 1,
        .y = 1 + TITLE_HEIGHT,
        .width = ws.ws_col,
        .height = MENU_AND_GAME_HEIGHT,
    };
    // display_menu(menu_game_position);
    display_game(menu_game_position);

    Position chat_position = {
        .x = 1,
        .y = 1 + TITLE_HEIGHT + MENU_AND_GAME_HEIGHT - VERTICAL_PADDING,
        .width = ws.ws_col,
        .height = ws.ws_row - TITLE_HEIGHT - MENU_AND_GAME_HEIGHT - VERTICAL_PADDING,
    };
    display_chat(chat_position, messages);
}

void display_window_too_small(struct winsize ws)
{
    move_cursor_to(1 + max(0, ws.ws_col - 35) / 2, 1 + (ws.ws_row - 1) / 2);
    printf("Window too small, please resize it\n");
}

void display_title(Position position)
{
    move_cursor_to(1 + (position.width - 5) / 2, position.y + VERTICAL_PADDING);
    color_printf("AWALÉ\n", BCYAN);
}

void display_menu(Position position)
{
    unsigned short first_column = (position.width - MENU_AND_GAME_WIDTH) / 2 + HORIZONTAL_PADDING;

    move_cursor_to(1 + (position.width - 7) / 2, position.y + VERTICAL_PADDING);
    color_printf("GENERAL\n", BWHITE);

    move_cursor_right(first_column);
    color_printf("send ", BYELLOW);
    printf("[");
    color_printf("player", BBLUE);
    printf("|");
    color_printf("all", BRED);
    printf("] message    send a message to a player / to everyone\n");

    move_cursor_right(first_column);
    color_printf("challenge ", BYELLOW);
    color_printf("player", BBLUE);
    printf("             challenge a player to a duel / accept\n");
    move_cursor_right(first_column);
    printf("                             the challenge from someone\n");

    move_cursor_right(first_column);
    color_printf("cancel_challenge ", BYELLOW);
    color_printf("player", BBLUE);
    printf("      cancel a challenge sent to a player\n");

    move_cursor_right(first_column);
    color_printf("historic ", BYELLOW);
    printf("(");
    color_printf("n", BCYAN);
    printf(")");
    printf("                 list your (n) past games\n");
    move_cursor_right(first_column);
    color_printf("replay ", BYELLOW);
    color_printf("game", BCYAN);
    printf("                  watch the replay of a game\n");

    printf("\n");
    move_cursor_right((position.width - 7) / 2);
    color_printf("IN GAME\n", BWHITE);

    move_cursor_right(first_column);
    color_printf("play ", BYELLOW);
    color_printf("slot", BGREEN);
    printf("                    choose which slot to play\n");

    printf("\n");
    move_cursor_right((position.width - 6) / 2);
    color_printf("REPLAY\n", BWHITE);

    move_cursor_right(first_column);
    color_printf("next ", BYELLOW);
    printf("                        display next move\n");
}

void display_game(Position position)
{
    int board[BOARD_SIZE] = {1, 2, 4, 6, 8, 12, 3, 0, 6, 8, 2, 3};
    char name_player1[] = "Mais what the fuck il est vraiment super archi mega long ton pseudo";
    unsigned int score_player1 = 69;
    char name_player2[] = "Joueur adverse";
    unsigned int score_player2 = 0;
    unsigned int player = 0;
    unsigned int turn = 22;

    int start_index, offset;

    char display_string[MENU_AND_GAME_WIDTH + 1];

    unsigned short is_my_turn = turn % 2 != player;

    sprintf(display_string, "Turn %d - ", turn);
    char who_is_playing[MENU_AND_GAME_WIDTH + 1];
    is_my_turn ? strcpy(who_is_playing, "Your time to play") : strcpy(who_is_playing, "Waiting for opponent to play");

    move_cursor_to((int) (1 + (position.width - strlen(display_string) - strlen(who_is_playing)) / 2), position.y + VERTICAL_PADDING);
    printf(display_string);
    color_printf(who_is_playing, is_my_turn ? BBLUE : MAGENTA);
    printf("\n\n\n");

    char opponent[] = " (Opponent)";
    offset = strlen(opponent);
    truncate_name(display_string, name_player2, MENU_AND_GAME_WIDTH - 2 * HORIZONTAL_PADDING - offset);
    move_cursor_right((int)(position.width - strlen(display_string) - offset) / 2);
    color_printf(display_string, MAGENTA);
    printf("%s\n\n", opponent);

    unsigned short board_left_column = (position.width - BOARD_WIDTH) / 2;

    move_cursor_right(board_left_column);
    printf("╭────┬────┬────┬────┬────┬────╮\n");

    sprintf(display_string, "%2d   ", score_player2);
    move_cursor_right((int)(board_left_column - strlen(display_string)));
    color_printf(display_string, MAGENTA);
    start_index = (player == 0 ? BOARD_SIZE : BOARD_SIZE / 2) - 1;
    for (int i = start_index; i >= start_index - BOARD_SIZE / 2 + 1; i--)
        printf("│ %2d ", board[i]);
    printf("│");
    color_printf("  ◀─╮\n", BBLACK);

    move_cursor_right(board_left_column);
    printf("├────┼────┼────┼────┼────┼────┤");
    color_printf("    │\n", BBLACK);

    sprintf(display_string, "%2d   ", score_player1);
    move_cursor_right((int)(board_left_column - strlen(display_string)));
    color_printf(display_string, BBLUE);
    start_index = (player == 0 ? 0 : BOARD_SIZE / 2);
    for (int i = start_index; i < start_index + BOARD_SIZE / 2; i++)
        printf("│ %2d ", board[i]);
    printf("│");
    color_printf("   ─╯\n", BBLACK);

    move_cursor_right(board_left_column);
    printf("╰────┴────┴────┴────┴────┴────╯\n");

    move_cursor_right(board_left_column);
    color_printf("  a    b    c    d    e    f\n\n", BBLACK);

    char you[] = " (You)";
    offset = strlen(you);
    truncate_name(display_string, name_player1, MENU_AND_GAME_WIDTH - 2 * HORIZONTAL_PADDING - offset);
    move_cursor_right((int)(position.width - strlen(display_string) - offset) / 2);
    color_printf(display_string, BBLUE);
    printf("%s\n\n", you);
}

void display_chat(Position position, Messages *messages)
{
    unsigned short first_column = (position.width - CHAT_WIDTH) / 2 + HORIZONTAL_PADDING;
    unsigned short content_length = CHAT_WIDTH - 2 * HORIZONTAL_PADDING - 2;

    move_cursor_to(1 + first_column, position.y + VERTICAL_PADDING);
    printf("╭");
    for (int i = 0; i < content_length; i++) printf("─");
    printf("╮\n");

    for (int i = 1; i < position.height - 1; i++)
    {
        move_cursor_right(first_column);
        printf("│");
        move_cursor_right(content_length);
        printf("│\n");
    }

    move_cursor_right(first_column);
    printf("╰");
    for (int i = 0; i < content_length; i++) printf("─");
    printf("╯\n");

    // TODO messages
    move_cursor_to(1 + first_column + 1, position.y + VERTICAL_PADDING + 1);
    printf("> ");

    fflush(stdout);
}

char *truncate_name(char *destination, char *source, size_t n)
{
    strncpy(destination, source, n);

    if (strlen(source) > n)
    {
        strcpy(destination + n - 3, "...");
    }

    return destination;
}
