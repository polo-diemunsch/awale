#include <string.h>
#include <stdlib.h> 

#include "game.h"
#include "../communication/communication.h"
#include "../../game/awale.h"
#include "../../lib/console.h"

Game *init_game(Client *player0, Client *player1)
{
   Game *game = create_game(player0->name, player1->name, 0);

   player0->game = game;
   player0->spectating = NULL;
   player1->game = game;
   player1->spectating = NULL;

   send_game_init_to_client(player0, game, 0);
   send_game_init_to_client(player1, game, 1);

   return game;
}

Game *find_game_by_name(Game **games, int actual, const char *name, char *error)
{
   int i;
   for (i = 0; i < actual; i++)
   {
      if (strcmp(name, games[i]->name) == 0)
         return games[i];
   }

   char game_name[BUF_SIZE - 43];
   strncpy(game_name, name, BUF_SIZE - 43);
   snprintf(error, BUF_SIZE - 1, "%sError: game %s%s%s not found.%s", ERROR_COLOR, INFORMATION_COLOR, game_name, ERROR_COLOR, RESET);
   return NULL;
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

void forfeit(Client *client, Client *clients, int actual, Game **games_playing, int *actual_games_playing, int disconnected)
{
   Game *game = client->game;
   if (game != NULL)
   {
      char error[BUF_SIZE - 1];
      unsigned char other_player = game->players[0].name == client->name;
      Client *opponent = find_client_by_name(clients, actual, game->players[other_player].name, error);
      game->winner = other_player + 2;
      if (!disconnected)
      {
         end_game(client, opponent, clients, actual, games_playing, actual_games_playing);
      }
      else
      {
         remove_spectators(game, clients, actual);
         remove_game(games_playing, game, actual_games_playing);
         send_game_end_to_client(opponent, game);
         opponent->game = NULL;
      }
   }
}

void end_game(Client *player0, Client *player1, Client *clients, int actual, Game **games_playing, int *actual_games_playing)
{
   Game *game = player0->game;

   remove_spectators(game, clients, actual);
   remove_game(games_playing, game, actual_games_playing);

   send_game_end_to_client(player0, game);
   player0->game = NULL;
   send_game_end_to_client(player1, game);
   player1->game = NULL;
}

void remove_spectators(Game *game, Client *clients, int actual)
{
   int i;
   for (i = 0; i < actual; i++)
   {
      if (clients[i].spectating == game)
      {
         send_game_end_to_client(&clients[i], game);
         clients[i].spectating = NULL;
      }
   }
}

void remove_game(Game **games, Game *game_to_remove, int *actual)
{
   int to_remove;
   for (to_remove = 0; to_remove < *actual; to_remove++)
   {
      if (games[to_remove] == game_to_remove)
         break;
   }
   if (to_remove < *actual)
   {
      /* we remove the client in the array */
      memmove(games + to_remove, games + to_remove + 1, (*actual - to_remove - 1) * sizeof(Game));
      /* number client - 1 */
      (*actual)--;
   }
}

int spectate(Client *client, Client *clients, int actual, Game **games_playing, int actual_games_playing, const char *target)
{
   char error[BUF_SIZE - 1];
   Game *game = NULL;

   if (client->game != NULL)
   {
      snprintf(error, BUF_SIZE - 1, "%sError: you can't spectate a game when you are already playing.%s", ERROR_COLOR, RESET);
      send_message_to_client(client, error);
      return -1;
   }

   Client *target_client = find_client_by_name(clients, actual, target, error);
   if (target_client != NULL)
   {
      if (target_client->game == NULL)
      {
         snprintf(error, BUF_SIZE - 1, "%sError: %s%s%s is not currently playing.%s", ERROR_COLOR, OPPONENT_COLOR, target_client->name, ERROR_COLOR, RESET);
         send_message_to_client(client, error);
         return -1;
      }
      game = target_client->game;
   }
   else
   {
      game = find_game_by_name(games_playing, actual_games_playing, target, error);
   }

   if (game == NULL)
   {
      snprintf(error, BUF_SIZE - 1, "%sError: player or game %s%s%s not found.%s", ERROR_COLOR, OPPONENT_COLOR, target, ERROR_COLOR, RESET);
      send_message_to_client(client, error);
      return -1;
   }

   client->spectating = game;
   send_game_init_to_client(client, game, 2);
   return 0;
}

int send_game(char * game_name, Client * client){
   FILE * f;
   f=fopen(game_name,"r");
   if (f==NULL){
      perror("This game does not exist.");
   } else {
              // Seek to the end of the file to determine its size
        fseek(f, 0, SEEK_END);
        long file_size = ftell(f); // Get the size of the file
        rewind(f); // Go back to the start of the file

        // Allocate memory for the file contents (+1 for the null terminator)
        char *file_content = (char *)malloc(file_size + 1);
        if (file_content == NULL) {
            perror("Failed to allocate memory.");
            fclose(f);
            return -1;
        }

        // Read the file into the buffer
        size_t bytes_read = fread(file_content, 1, file_size, f);
        if (bytes_read != file_size) {
            perror("Error reading the file.");
            free(file_content);
            fclose(f);
            return -1;
        }

        file_content[file_size] = '\0'; // Null-terminate the string

        // Print or process the content as needed
        printf("File Content:\n%s\n", file_content);
        send_message_to_client(client,file_content);
        // Clean up
        free(file_content);
        fclose(f);
   }
    return 0;
}

