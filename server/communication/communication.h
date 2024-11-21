#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../sock.h"
#include "../client/client.h"
#include "../../game/awale.h"

int read_client(SOCKET sock, char *buffer);
void write_client(SOCKET sock, const char *buffer, size_t n);
void send_game_init_to_client(Client *receiver, Game *game, unsigned char which_player_is_it);
void send_game_state_to_client(Client *receiver, Game *game);
void send_game_state_to_spectators(Client *clients, int actual, Game *game);
void send_game_end_to_client(Client *receiver, Game *game);
void send_message_to_client(Client *receiver, char *buffer);

void send_message(Client *clients, Client *sender, int actual, const char *target, const char *buffer);
void send_message_from_client_to_client(Client *receiver, Client *sender, const char *buffer);
void send_message_to_all_clients(Client *clients, Client *sender, int actual, const char *buffer);

#endif /* guard */
