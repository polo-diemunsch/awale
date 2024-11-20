#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "communication.h"
#include "../../lib/console.h"

int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

void write_client(SOCKET sock, const char *buffer, size_t n)
{
   if(send(sock, buffer, n, 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

void send_game_init_to_client(Client *receiver, Game *game, unsigned char which_player_is_it)
{
   char buffer[BUF_SIZE];
   *buffer = GAME_INIT;
   size_t n = serialize_game(game, which_player_is_it, buffer + 1);
   write_client(receiver->sock, buffer, n + 1);
}

void send_game_state_to_client(Client *receiver, Game *game)
{
   char buffer[BUF_SIZE];
   *buffer = GAME_STATE;
   size_t n = serialize_game_state(game, buffer + 1);
   write_client(receiver->sock, buffer, n + 1);
}

void send_game_end_to_client(Client *receiver, Game *game)
{
   char buffer[BUF_SIZE];
   *buffer = GAME_END;
   char message[BUF_SIZE - 1];

   unsigned char player = (game->players[1].name == receiver->name);
   unsigned char opponent = (player + 1) % 2;
   
   if (game->winner == player)
      snprintf(message, BUF_SIZE - 1, "%sYou won against %s%s%s (%u - %u)!%s", INFORMATION_COLOR, OPPONENT_COLOR, game->players[opponent].name, INFORMATION_COLOR, game->players[player].score, game->players[opponent].score, RESET);
   else if (game->winner == opponent)
      snprintf(message, BUF_SIZE - 1, "%sYou lost against %s%s%s (%u - %u)!%s", INFORMATION_COLOR, OPPONENT_COLOR, game->players[opponent].name, INFORMATION_COLOR, game->players[player].score, game->players[opponent].score, RESET);
   else if (game->winner == player + 2)
      snprintf(message, BUF_SIZE - 1, "%sYou won by forfeit against %s%s%s (%u - %u)!%s", INFORMATION_COLOR, OPPONENT_COLOR, game->players[opponent].name, INFORMATION_COLOR, game->players[player].score, game->players[opponent].score, RESET);
   else if (game->winner == opponent + 2)
      snprintf(message, BUF_SIZE - 1, "%sYou lost by forfeit against %s%s%s (%u - %u)!%s", INFORMATION_COLOR, OPPONENT_COLOR, game->players[opponent].name, INFORMATION_COLOR, game->players[player].score, game->players[opponent].score, RESET);
   else
      snprintf(message, BUF_SIZE - 1, "%sThe game against %s%s%s ends in a draw (%u - %u).%s", INFORMATION_COLOR, OPPONENT_COLOR, game->players[opponent].name, INFORMATION_COLOR, game->players[player].score, game->players[opponent].score, RESET);

   size_t n = write_string_to_buffer(message, buffer + 1);
   write_client(receiver->sock, buffer, n + 1);
}

void send_message_to_client(Client *receiver, char *message)
{
   char buffer[BUF_SIZE];
   *buffer = MESSAGE;
   size_t n = write_string_to_buffer(message, buffer + 1);
   write_client(receiver->sock, buffer, n + 1);
}

void send_message(Client *clients, Client *sender, int actual, const char*target, const char *buffer)
{
   if (strcmp(target, "all") == 0)
   {
      send_message_to_all_clients(clients, sender, actual, buffer);
   }
   else
   {
      char error[BUF_SIZE - 1];
      Client *receiver = find_client_by_name(clients, actual, target, error);
      if (receiver != NULL)
      {
         if (sender->sock != receiver->sock)
            send_message_from_client_to_client(receiver, sender, buffer);
         else
            send_message_to_client(sender, FRIENDS);
      }
      else
      {
         send_message_to_client(sender, error);
      }
   }
}

void send_message_from_client_to_client(Client *receiver, Client *sender, const char *buffer)
{
   char message[BUF_SIZE - 1];

   snprintf(message, BUF_SIZE - 1, "%sfrom %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, sender->name, BWHITE, buffer, RESET);
   send_message_to_client(receiver, message);

   snprintf(message, BUF_SIZE - 1, "%sto %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, receiver->name, BWHITE, buffer, RESET);
   send_message_to_client(sender, message);
}

void send_message_to_all_clients(Client *clients, Client *sender, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE - 1];
   for (i = 0; i < actual; i++)
   {
      snprintf(message, BUF_SIZE - 1, "%s%s%s: %s", clients[i].sock != sender->sock ? OPPONENT_COLOR : OWN_COLOR, sender->name, RESET, buffer);
      send_message_to_client(&clients[i], message);
   }
}