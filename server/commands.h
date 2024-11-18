#ifndef COMMANDS_H
#define COMMANDS_H

#include "server.h"

#define FRIENDS "\033[36mhttps://www.santemagazine.fr/psycho-sexo/psycho/10-facons-de-se-faire-des-amis-178690\033[0m"

void send_message(Client *clients, Client *sender, int actual, const char *target, const char *buffer);
void send_message_from_client_to_client(Client *receiver, Client *sender, const char *buffer);
void send_message_to_all_clients(Client *clients, Client *sender, int actual, const char *buffer);

int challenge(Client *challenger, Client *challengee, Challenge (*challenges)[MAX_CLIENTS * MAX_CLIENTS]);

int play(Client *client, char *move);

#endif /* guard */
