#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "game/game.h"
#include "communication/communication.h"
#include "challenge/challenge.h"
#include "../lib/console.h"

void init(void)
{
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];

   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

   /* the index for the array */
   int actual_challenge = 0;
   /* an array for all challenges */
   Challenge challenges[MAX_CLIENTS * MAX_CLIENTS];

   /* the index for the array */
   int actual_games_playing = 0;
   /* an array for all games playing */
   Game *games_playing[MAX_CLIENTS * MAX_CLIENTS];

   fd_set rdfs;

   while(1)
   {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for(i = 0; i < actual; i++)
      {
         FD_SET(clients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1)
      {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if(FD_ISSET(STDIN_FILENO, &rdfs))
      {
         /* stop process when type on keyboard */
         break;
      }
      else if(FD_ISSET(sock, &rdfs))
      {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         unsigned int sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR)
         {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if(read_client(csock, buffer) == -1)
         {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, NAME_SIZE);
         c.game = NULL;
         clients[actual] = c;
         actual++;

         char message[BUF_SIZE - 1];
         snprintf(message, BUF_SIZE, "%sWelcome to Awalé %s%s%s!%s", BYELLOW, OWN_COLOR, c.name, BYELLOW, RESET);
         send_message_to_client(&c, message);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client *client = &clients[i];
               int c = read_client(clients[i].sock, buffer);
               /* client disconnected */
               if(c == 0)
               {
                  closesocket(clients[i].sock);
                  remove_client(clients, i, &actual, games_playing, &actual_games_playing);
                  // strncpy(buffer, client.name, BUF_SIZE - 1);
                  // strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  // send_message_to_all_clients(clients, client, actual, buffer, 1);
                  break;
               }

               printf("%s: %s\n", client->name, buffer);

               char buffer_copy[BUF_SIZE], *command, *remainder;
               strcpy(buffer_copy, buffer);

               command = strtok_r(buffer_copy, " ", &remainder);

               if (strcmp(command, "online") == 0 || strcmp(command, "o") == 0)
               {
                  char message[BUF_SIZE - 1], *put = message;
                  put += snprintf(put, BUF_SIZE - 1 - (put - message), "Online players:                                                      ");
                  for(int i = 0; i < actual; ++i)
                  {
                     put += snprintf(put, BUF_SIZE - 1 - (put - message), "- %s%-67s%s", client->sock == clients[i].sock ? OWN_COLOR : OPPONENT_COLOR, clients[i].name, RESET); 
                  }
                  send_message_to_client(client, message);
               }
               else if (strcmp(command, "send") == 0 || strcmp(command, "s") == 0)
               {
                  char error[BUF_SIZE - 1];

                  char *target = strtok_r(NULL, " ", &remainder);
                  if (target == NULL)
                  {
                     snprintf(error, BUF_SIZE - 1, "%sError: you must specify a target for your message.%s", ERROR_COLOR, RESET);
                     send_message_to_client(client, error);
                     break;
                  }

                  send_message(clients, client, actual, target, remainder);
               }
               else if (strcmp(command, "challenge") == 0 || strcmp(command, "c") == 0)
               {
                  char error[BUF_SIZE - 1];

                  char *target = strtok_r(NULL, " ", &remainder);
                  if (target == NULL)
                  {
                     snprintf(error, BUF_SIZE - 1, "%sError: you must specify a player you want to challenge.%s", ERROR_COLOR, RESET);
                     send_message_to_client(client, error);
                     break;
                  }

                  Client *challengee = find_client_by_name(clients, actual, target, error);
                  if (challengee == NULL)
                  {
                     send_message_to_client(client, error);
                     break;
                  }

                  if (challenge(client, challengee, challenges, &actual_challenge) == 1)
                  {
                     games_playing[actual_games_playing] = init_game(client, challengee);
                     actual_games_playing++;
                  }
               }
               else if (strcmp(command, "decline_challenge") == 0 || strcmp(command, "dc") == 0)
               {
                  char error[BUF_SIZE - 1];

                  char *target = strtok_r(NULL, " ", &remainder);
                  if (target == NULL)
                  {
                     snprintf(error, BUF_SIZE - 1, "%sError: you must specify a player whose challenge you want to decline.%s", ERROR_COLOR, RESET);
                     send_message_to_client(client, error);
                     break;
                  }

                  Client *challengee = find_client_by_name(clients, actual, target, error);
                  if (challengee == NULL)
                  {
                     send_message_to_client(client, error);
                     break;
                  }

                  decline_challenge(client, challengee, challenges, &actual_challenge);
               }
               else if (strcmp(command, "list_games") == 0 || strcmp(command, "lg") == 0)
               {
                  char message[BUF_SIZE - 1], *put = message;
                  put += snprintf(put, BUF_SIZE - 1 - (put - message), "Current games:                                                       ");;
                  for(int i = 0; i < actual_games_playing; ++i)
                  {
                     put += snprintf(put, BUF_SIZE - 1 - (put - message), "- %s%-67s%s", client->game == games_playing[i] ? OWN_COLOR : OPPONENT_COLOR, games_playing[i]->name, RESET); 
                  }
                  send_message_to_client(client, message);
               }
               else if (strcmp(command, "spectate") == 0 || strcmp(command, "sp") == 0)
               {
                  char error[BUF_SIZE - 1];

                  char *target = strtok_r(NULL, " ", &remainder);
                  if (target == NULL)
                  {
                     snprintf(error, BUF_SIZE - 1, "%sError: you must specify a player or a game you want to spectate.%s", ERROR_COLOR, RESET);
                     send_message_to_client(client, error);
                     break;
                  }

                  spectate(client, clients, actual, games_playing, actual_games_playing, target);
               }
               else if (strcmp(command, "play") == 0 || strcmp(command, "p") == 0)
               {
                  char *move = strtok_r(NULL, " ", &remainder);
                  int play_result = play(client, move);

                  if (play_result == -1)
                     break;

                  Game *game = client->game;
                  char error[BUF_SIZE - 1];
                  unsigned char other_player = (game->players[0].name == client->name);
                  Client *opponent = find_client_by_name(clients, actual, game->players[other_player].name, error);

                  if (play_result == 0)
                  {
                     send_game_state_to_spectators(clients, actual, game);
                     send_game_state_to_client(client, game);
                     send_game_state_to_client(opponent, game);
                  }
                  else if (play_result == 1)
                  {
                     end_game(client, opponent, clients, actual, games_playing, &actual_games_playing);
                  }
               }
               else if (strcmp(command, "forfeit") == 0 || strcmp(command, "ff") == 0)
               {
                  forfeit(client, clients, actual, games_playing, &actual_games_playing, 0);
               }
               else if (strcmp(command, "home") == 0 || strcmp(command, "h") == 0)
               {
                  client->spectating = NULL;
               }
               else
               {
                  char message[BUF_SIZE - 1];
                  char truncated_command[BUF_SIZE - 47];
                  strncpy(truncated_command, command, BUF_SIZE - 47);
                  snprintf(message, BUF_SIZE - 1, "%sError: no command named %s%s%s !%s", ERROR_COLOR, BYELLOW, truncated_command, ERROR_COLOR, RESET);
                  send_message_to_client(client, message);
               }
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if (listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

void end_connection(int sock)
{
   closesocket(sock);
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
