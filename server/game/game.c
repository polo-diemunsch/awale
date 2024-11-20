#include <string.h>

#include "game.h"
#include "../communication/communication.h"
#include "../../game/awale.h"
#include "../../lib/console.h"

void init_game(Client *player0, Client *player1)
{
   Game *game = create_game(player0->name, player1->name, 0);
   player0->game = game;
   player1->game = game;
   send_game_init_to_client(player0, game, 0);
   send_game_init_to_client(player1, game, 1);
}

int play(Client *client, char *move)
{
   Game *game = client->game;
   char message[BUF_SIZE - 1];

   if (game == NULL)
   {
      snprintf(message, BUF_SIZE - 1, "%sError: you are not in a game.%s", ERROR_COLOR, RESET);
      send_message_to_client(client, message);
      return -1;
   }

   if (client->name != game->players[game->turn].name)
   {
      snprintf(message, BUF_SIZE - 1, "%sError: it's not your turn to play.%s", ERROR_COLOR, RESET);
      send_message_to_client(client, message);
      return -1;
   }

   if (strlen(move) != 1)
   {
      snprintf(message, BUF_SIZE - 1, "%sError: invalid move %s%s%s.%s", ERROR_COLOR, OWN_COLOR, move, ERROR_COLOR, RESET);
      send_message_to_client(client, message);
      return -1;
   }

   char player_slot = -1;
   if (move[0] >= 65 && move[0] <= 70)
      player_slot = move[0] - 65;
   else if (move[0] >= 97 && move[0] <= 102)
      player_slot = move[0] - 97;
   
   if (player_slot == -1)
   {
      snprintf(message, BUF_SIZE - 1, "%sError: invalid move %s%s%s.%s", ERROR_COLOR, OWN_COLOR, move, ERROR_COLOR, RESET);
      send_message_to_client(client, message);
      return -1;
   }

   unsigned char slot = player_slot + game->turn * (BOARD_SIZE / 2);
   int round_result = execute_round(game, game->turn, slot, message);
   if (round_result == -1)
   {
      send_message_to_client(client, message);
      return -1;
   }

   return round_result;
}
