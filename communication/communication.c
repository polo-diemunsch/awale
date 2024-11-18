#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "communication.h"

size_t write_string_to_buffer(const char *string, char *buffer)
{
    size_t length = strlen(string) + 1;
    strcpy(buffer, string);
    return length;
}

size_t read_string_from_buffer(char **string, const char *buffer)
{
    if (*string != NULL)
        free(*string);

    size_t length = strlen(buffer) + 1;

    *string = malloc(length * sizeof(char));
    strcpy(*string, buffer);

    return length;
}

size_t serialize_player(Player *player, char *buffer)
{
    size_t length = write_string_to_buffer(player->name, buffer);
    *(buffer + length) = player->score;
    return length + 1;
}

size_t unserialize_player(Player *player, const char *buffer)
{
    size_t length = read_string_from_buffer(&(player->name), buffer);
    player->score = *(buffer + length);
    return length + 1;
}

size_t serialize_game(Game *game, unsigned char which_player_is_it, char *buffer)
{
    size_t length = 0;

    length += serialize_player(&(game->players[0]), buffer + length);
    length += serialize_player(&(game->players[1]), buffer + length);

    strncpy(buffer + length, game->board, BOARD_SIZE);
    length += BOARD_SIZE;

    int round = htons(game->round);
    memcpy(buffer + length, &round, sizeof(int));
    length += sizeof(int) / sizeof(char);

    *((char *)(buffer + length)) = game->turn;
    length++;

    *((char *)(buffer + length)) = game->direction;
    length++;

    *((char *)(buffer + length)) = which_player_is_it;
    length++;

    return length;
}

unsigned char unserialize_game(Game *game, const char *buffer)
{
    size_t length = 0;

    length += unserialize_player(&(game->players[0]), buffer + length);
    length += unserialize_player(&(game->players[1]), buffer + length);

    strncpy(game->board, buffer + length, BOARD_SIZE);
    length += BOARD_SIZE;

    int round;
    memcpy(&round, buffer + length, sizeof(int));
    game->round = ntohs(round);
    length += sizeof(int) / sizeof(char);

    game->turn = *(buffer + length);
    length++;

    game->direction = *(buffer + length);
    length++;

    unsigned char who_am_i = *(buffer + length);

    return who_am_i;
}

size_t serialize_game_init_history (Game *game, char *buffer)
{
    size_t length = 0;

    length += serialize_player(&(game->players[0]), buffer + length);
    length += serialize_player(&(game->players[1]), buffer + length);

    strncpy(buffer + length, game->board, BOARD_SIZE);
    length += BOARD_SIZE;

    int round = htons(game->round);
    memcpy(buffer + length, &round, sizeof(int));
    length += sizeof(int) / sizeof(char);

    *((char *)(buffer + length)) = game->turn;
    length++;

    *((char *)(buffer + length)) = game->direction;
    length++;

    return length;
}

