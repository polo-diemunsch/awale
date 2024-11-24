#ifndef GAME_H
#define GAME_H

#include "../client/client.h"

Game *init_game(Client *player0, Client *player1);
Game *find_game_by_name(Game **games, int actual, const char *name, char *error);

int play(Client *client, char *move);
void forfeit(Client *client, Client *clients, int actual, Game **games_playing, int *actual_game_playing, int disconnected);
void end_game(Client *player0, Client *player1, Client *clients, int actual, Game **games_playing, int *actual_games_playing);
void remove_spectators(Game *game, Client *clients, int actual);
void remove_game(Game **games, Game *game_to_remove, int *actual);

int spectate(Client *client, Client *clients, int actual, Game **games_playing, int actual_games_playing, const char *target);

#endif /* guard */