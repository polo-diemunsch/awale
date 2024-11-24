#ifndef CLIENT_H
#define CLIENT_H

#include "../sock.h"
#include "../../serialization/serialization.h"

typedef struct {
   SOCKET sock;
   char name[NAME_SIZE];
   char password[PASSWORD_SIZE];
   Game *game;
   Game *spectating;
} Client;

Client *find_client_by_name(Client *clients, int actual, const char *name, char *error);
void remove_client(Client *clients, int to_remove, int *actual, Game **games_playing, int *actual_games_playing);
void clear_clients(Client *clients, int actual);

#endif /* guard */
