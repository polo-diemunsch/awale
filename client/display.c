#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "display.h"
#include "../lib/console.h"
#include "../lib/math.h"

void update_interface(struct winsize ws, Messages *messages, Game *game, char who_am_i)
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
    if (game == NULL)
        display_menu(menu_game_position);
    else
        display_game(menu_game_position, game, who_am_i);

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

    move_cursor_to(1, position.y + VERTICAL_PADDING);

    move_cursor_right(first_column);
    color_printf("online ", BYELLOW);
    printf("                      show online players\n");
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
    printf("                             the challenge from a player\n");

    move_cursor_right(first_column);
    color_printf("decline_challenge ", BYELLOW);
    color_printf("player", BBLUE);
    printf("     decline a challenge from a player /\n");
    move_cursor_right(first_column);
    printf("                             cancel a challenge sent to a player\n");

    move_cursor_right(first_column);
    color_printf("play ", BYELLOW);
    color_printf("slot", BGREEN);
    printf("                    choose which slot to play\n");

    move_cursor_right(first_column);
    color_printf("forfeit", BYELLOW);
    printf("                      forfeit the game\n");

    move_cursor_right(first_column);
    color_printf("list_games", BYELLOW);
    printf("                   list games currently playing\n");

    move_cursor_right(first_column);
    color_printf("spectate ", BYELLOW);
    printf("[");
    color_printf("player", BBLUE);
    printf("|");
    color_printf("game", BCYAN);
    printf("]       spectate a player or a game\n");

    move_cursor_right(first_column);
    color_printf("history ", BYELLOW);
    printf("(");
    color_printf("n", BCYAN);
    printf(")");
    printf("                 list your past games\n");

    move_cursor_right(first_column);
    color_printf("home", BYELLOW);
    printf("                         stop spectating a game\n");
}

void display_game(Position position, Game *game, unsigned char who_am_i)
{
    unsigned char am_i_playing = who_am_i < 2;
    unsigned char bottom_player = am_i_playing ? who_am_i : 0;
    unsigned char top_player = (bottom_player + 1) % 2;

    int start_index, offset;

    char display_string[MENU_AND_GAME_WIDTH + 1];

    unsigned char is_my_turn = game->turn == who_am_i;

    sprintf(display_string, "Turn %d - ", game->round);
    char who_is_playing[MENU_AND_GAME_WIDTH + 1];
    if (is_my_turn)
        strcpy(who_is_playing, "Your time to play");
    else
        sprintf(who_is_playing, "Waiting for %s to play", game->players[game->turn].name);

    move_cursor_to((int) (1 + (position.width - strlen(display_string) - strlen(who_is_playing)) / 2), position.y + VERTICAL_PADDING + 1);
    printf("%s", display_string);
    color_printf(who_is_playing, is_my_turn ? OWN_COLOR : OPPONENT_COLOR);
    printf("\n\n\n");

    char opponent[] = " (Opponent)";
    offset = am_i_playing ? strlen(opponent) : 0;
    truncate_name(display_string, game->players[top_player].name, MENU_AND_GAME_WIDTH - 2 * HORIZONTAL_PADDING - offset);
    move_cursor_right((int)(position.width - get_visual_length(display_string) - offset) / 2);
    color_printf(display_string, OPPONENT_COLOR);
    if (am_i_playing)
        printf("%s", opponent);
    printf("\n\n");

    unsigned short board_left_column = (position.width - BOARD_WIDTH) / 2;

    move_cursor_right(board_left_column);
    printf("╭────┬────┬────┬────┬────┬────╮\n");

    sprintf(display_string, "%2d   ", game->players[top_player].score);
    move_cursor_right((int)(board_left_column - strlen(display_string)));
    color_printf(display_string, OPPONENT_COLOR);
    start_index = (bottom_player == 0 ? BOARD_SIZE : BOARD_SIZE / 2) - 1;
    for (int i = start_index; i >= start_index - BOARD_SIZE / 2 + 1; i--)
        printf("│ %2d ", game->board[i]);
    printf("│");
    color_printf("  ◀─╮\n", BBLACK);

    move_cursor_right(board_left_column);
    printf("├────┼────┼────┼────┼────┼────┤");
    color_printf("    │\n", BBLACK);

    sprintf(display_string, "%2d   ", game->players[bottom_player].score);
    move_cursor_right((int)(board_left_column - strlen(display_string)));
    color_printf(display_string, OWN_COLOR);
    start_index = (bottom_player == 0 ? 0 : BOARD_SIZE / 2);
    for (int i = start_index; i < start_index + BOARD_SIZE / 2; i++)
        printf("│ %2d ", game->board[i]);
    printf("│");
    color_printf("   ─╯\n", BBLACK);

    move_cursor_right(board_left_column);
    printf("╰────┴────┴────┴────┴────┴────╯\n");

    move_cursor_right(board_left_column);
    color_printf("  a    b    c    d    e    f\n\n", BBLACK);

    char you[] = " (You)";
    offset = am_i_playing ? strlen(you) : 0;
    truncate_name(display_string, game->players[bottom_player].name, MENU_AND_GAME_WIDTH - 2 * HORIZONTAL_PADDING - offset);
    move_cursor_right((int)(position.width - get_visual_length(display_string) - offset) / 2);
    color_printf(display_string, OWN_COLOR);
    if (am_i_playing)
        printf("%s", you);
    printf("\n\n");
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

    move_cursor_to(1, position.y + VERTICAL_PADDING + 1);
    int index, lines_left;

    if (messages->newest_displayed_message_index != messages->newest_message_index)
    {
        messages->newest_displayed_message_index = messages->newest_message_index;
        index = messages->newest_message_index;
        lines_left = position.height - VERTICAL_PADDING - 2;
        while (lines_left > 0 && index != messages->oldest_message_index)
        {
            lines_left -= get_visual_length(messages->messages[index]) / (content_length + 1) + 1;
            if (lines_left >= 0)
                index = mod(index - 1, MESSAGES_COUNT);
        }
        messages->oldest_displayed_message_index = index;
    }

    index = mod(messages->oldest_displayed_message_index + 1, MESSAGES_COUNT);
    while (index != mod(messages->newest_displayed_message_index + 1, MESSAGES_COUNT))
    {
        multiline_print(messages->messages[index], first_column, content_length);
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
