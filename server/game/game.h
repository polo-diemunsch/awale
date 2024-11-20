#ifndef GAME_H
#define GAME_H

#include "../client/client.h"

void init_game(Client *player0, Client *player1);

int play(Client *client, char *move);

#endif /* guard */