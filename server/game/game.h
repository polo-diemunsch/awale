#ifndef GAME_H
#define GAME_H

#include "../client/client.h"

Game *init_game(Client *player0, Client *player1);

int play(Client *client, char *move);
void forfeit(Client *client, Client *clients, int actual, int disconnected);
void end_game(Client *player0, Client *player1);

#endif /* guard */