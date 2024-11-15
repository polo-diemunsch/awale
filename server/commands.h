#ifndef COMMANDS_H
#define COMMANDS_H

#include "server.h"

void send_message(Client *clients, Client sender, int actual, const char *target, const char *buffer);
void send_message_from_client_to_client(Client receiver, Client sender, const char *buffer);
void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer);

int challenge(Client *challenger, Client *challengee, Challenge (*challenges)[MAX_CLIENTS * MAX_CLIENTS]);

#endif /* guard */
