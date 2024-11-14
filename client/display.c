#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "../lib/console.h"
#include "../lib/math.h"

void update_interface(struct winsize ws, Messages messages)
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
    display_menu(menu_game_position);
    // display_game(menu_game_position);

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
    char name_player1[] = "Mais what the fuck il est vraiment super archi méga long ton pseudo";
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
    color_printf(who_is_playing, is_my_turn ? OWN_COLOR : OPPONENT_COLOR);
    printf("\n\n\n");

    char opponent[] = " (Opponent)";
    offset = strlen(opponent);
    truncate_name(display_string, name_player2, MENU_AND_GAME_WIDTH - 2 * HORIZONTAL_PADDING - offset);
    move_cursor_right((int)(position.width - get_visual_length(display_string) - offset) / 2);
    color_printf(display_string, OPPONENT_COLOR);
    printf("%s\n\n", opponent);

    unsigned short board_left_column = (position.width - BOARD_WIDTH) / 2;

    move_cursor_right(board_left_column);
    printf("╭────┬────┬────┬────┬────┬────╮\n");

    sprintf(display_string, "%2d   ", score_player2);
    move_cursor_right((int)(board_left_column - strlen(display_string)));
    color_printf(display_string, OPPONENT_COLOR);
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
    color_printf(display_string, OWN_COLOR);
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
    move_cursor_right((int)(position.width - get_visual_length(display_string) - offset) / 2);
    color_printf(display_string, OWN_COLOR);
    printf("%s\n\n", you);
}

void display_chat(Position position, Messages messages)
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

    move_cursor_to(1, position.y + VERTICAL_PADDING + 1);
    int index, lines_left;

    if (messages.newest_displayed_message_index != messages.newest_message_index)
    {
        messages.newest_displayed_message_index = messages.newest_message_index;
        index = messages.newest_message_index;
        lines_left = position.height - VERTICAL_PADDING - 3;
        while (lines_left > 0 && index != messages.oldest_message_index)
        {
            lines_left -= get_visual_length(messages.messages[index]) / (content_length + 1) + 1;
            if (lines_left >= 0)
                index = mod(index - 1, MESSAGES_COUNT);
        }
        messages.oldest_displayed_message_index = index;
    }

    index = mod(messages.oldest_displayed_message_index + 1, MESSAGES_COUNT);
    while (index != mod(messages.newest_displayed_message_index + 1, MESSAGES_COUNT))
    {
        multiline_print(messages.messages[index], first_column, content_length);
        index = mod(index + 1, MESSAGES_COUNT);
    }

    move_cursor_right(first_column + 1);
    printf("> ");

    fflush(stdout);
}

int get_visual_length(const char *string)
{
    int visual_length = 0;
    int i = 0;
    
    while (string[i] != '\0')
    {
        if (string[i] == '\033')  // ANSI escape sequence
        {
            while (string[i] && string[i] != 'm') 
                i++;
            i++;  // skip the 'm'
            continue;
        }
        
        // handle UTF-8 multi-byte chars
        if ((string[i] & 0xC0) != 0x80)  // not a continuation byte
            visual_length++;

        i++;
    }

    return visual_length;
}

int get_string_length_for_visual_length(const char *string, int visual_length)
{
    int i = 0;
    
    while (string[i] != '\0' && visual_length >= 0)
    {
        if (string[i] == '\033')  // ANSI escape sequence
        {
            while (string[i] && string[i] != 'm') 
                i++;
            i++;  // skip the 'm'
            continue;
        }
        
        // handle UTF-8 multi-byte chars
        if ((string[i] & 0xC0) != 0x80)  // not a continuation byte
            visual_length--;

        i++;
    }

    return visual_length < 0 ? i - 1 : i;
}

char *truncate_name(char *destination, const char *source, size_t n)
{
    int string_length = get_string_length_for_visual_length(source, n);
    strncpy(destination, source, string_length);

    if (strlen(source) > string_length)
        strcpy(destination + get_string_length_for_visual_length(destination, n - 3), "...");

    destination[string_length] = '\0';

    return destination;
}

void multiline_print(const char *string, unsigned short first_column, unsigned short content_length)
{
    int string_length = strlen(string);
    int visual_length = get_visual_length(string);
    int string_position = 0;
    int chars_printed = 0;

    char *displayed_string = malloc((string_length + 1) * sizeof(char));

    while (chars_printed < visual_length)
    {
        int remaining = visual_length - chars_printed;
        int chunk_size = get_string_length_for_visual_length(string + string_position, (remaining < content_length) ? remaining : content_length);
        
        strncpy(displayed_string, string + string_position, chunk_size);
        displayed_string[chunk_size] = '\0';

        move_cursor_right(first_column + 1);
        printf("%s\n", displayed_string);

        string_position += chunk_size;
        chars_printed += content_length;
    }

    free(displayed_string);
}
