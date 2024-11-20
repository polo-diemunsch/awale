#ifndef CLIENT_H
#define CLIENT_H

#include "../sock.h"
#include "../../serialization/serialization.h"

typedef struct {
   SOCKET sock;
   char name[NAME_SIZE];
   Game *game;
} Client;

Client *find_client_by_name(Client *clients, int actual, const char *name, char *error);
void remove_client(Client *clients, int to_remove, int *actual);
void clear_clients(Client *clients, int actual);

#endif /* guard */
