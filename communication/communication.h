#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdio.h>

#include "../game/awale.h"

#define CRLF        "\r\n"
#define PORT        1977
#define MAX_CLIENTS 100

#define BUF_SIZE    1024
#define NAME_SIZE   128

typedef enum {
    MESSAGE,
    GAME_INIT,
    GAME_STATE,
} CommunicationType;

size_t write_string_to_buffer(const char *string, char *buffer);

size_t read_string_from_buffer(char **string, const char *buffer);

size_t serialize_player(Player *player, char *buffer);

size_t unserialize_player(Player *player, const char *buffer);

size_t serialize_game(Game *game, unsigned char which_player_is_it, char *buffer);

size_t serialize_game_init_history (Game *game, char *buffer);

unsigned char unserialize_game(Game *game, const char *buffer);

size_t serialize_game_state(Game *game, char *buffer);

size_t unserialize_game_state(Game *game, const char *buffer);

#endif /* guard */
