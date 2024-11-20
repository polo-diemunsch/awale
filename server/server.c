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
                  remove_client(clients, i, &actual);
                  // strncpy(buffer, client.name, BUF_SIZE - 1);
                  // strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  // send_message_to_all_clients(clients, client, actual, buffer, 1);
                  break;
               }

               printf("%s: %s\n", client->name, buffer);

               char buffer_copy[BUF_SIZE], *command, *remainder;
               strcpy(buffer_copy, buffer);

               command = strtok_r(buffer_copy, " ", &remainder);

               if (strcmp(command, "send") == 0 || strcmp(command, "s") == 0)
               {
                  char *target = strtok_r(NULL, " ", &remainder);
                  send_message(clients, client, actual, target, remainder);
               }
               else if (strcmp(command, "challenge") == 0 || strcmp(command, "c") == 0)
               {
                  char *target = strtok_r(NULL, " ", &remainder);
                  char error[BUF_SIZE - 1];
                  Client *challengee = find_client_by_name(clients, actual, target, error);
                  if (challengee != NULL)
                  {
                     if (challenge(client, challengee, challenges, &actual_challenge) == 1)
                        init_game(client, challengee);
                  }
                  else
                  {
                     send_message_to_client(client, error);
                  }
               }
               else if (strcmp(command, "decline_challenge") == 0 || strcmp(command, "dc") == 0)
               {
                  char *target = strtok_r(NULL, " ", &remainder);
                  char error[BUF_SIZE - 1];
                  Client *challengee = find_client_by_name(clients, actual, target, error);
                  if (challengee != NULL)
                  {
                     decline_challenge(client, challengee, challenges, &actual_challenge);
                  }
                  else
                  {
                     send_message_to_client(client, error);
                  }
               }
               else if (strcmp(command, "play") == 0 || strcmp(command, "p") == 0)
               {
                  char *move = strtok_r(NULL, " ", &remainder);
                  int play_result = play(client, move);
                  if (play_result != -1)
                  {
                     Game *game = client->game;
                     char error[BUF_SIZE - 1];
                     unsigned char other_player = (game->players[0].name == client->name);
                     Client *opponent = find_client_by_name(clients, actual, game->players[other_player].name, error);

                     if (play_result == 0)
                     {
                        send_game_state_to_client(client, game);
                        send_game_state_to_client(opponent, game);
                     }
                     else if (play_result == 1)
                     {
                        client->game = NULL;
                        send_game_end_to_client(client, game);
                        opponent->game = NULL;
                        send_game_end_to_client(opponent, game);
                     }
                  }
               }
               else if (strcmp(command, "forfeit") == 0 || strcmp(command, "ff") == 0)
               {
                  Game *game = client->game;
                  if (game != NULL)
                  {
                     char error[BUF_SIZE - 1];
                     unsigned char other_player = game->players[0].name == client->name;
                     Client *opponent = find_client_by_name(clients, actual, game->players[other_player].name, error);
                     game->winner = other_player + 2;
                     client->game = NULL;
                     send_game_end_to_client(client, game);
                     opponent->game = NULL;
                     send_game_end_to_client(opponent, game);
                  }
               }
               else if (strcmp(command, "online") == 0 || strcmp(command, "o") == 0)
               {
                  char message[BUF_SIZE - 1], *put = message;
                  snprintf(put, BUF_SIZE - 1 - (put - message), "Online players: ");
                  put += 16;
                  for(int i = 0; i < actual; ++i){
                     if (i > 0)
                        put += snprintf(put, BUF_SIZE - 1 - (put - message), " - "); 
                     put += snprintf(put, BUF_SIZE - 1 - (put - message), "%s%s%s", client->sock == clients[i].sock ? OWN_COLOR : OPPONENT_COLOR, clients[i].name, RESET); 
                  }
                  send_message_to_client(client, message);
               }
               else
               {
                  char message[BUF_SIZE - 1];
                  char truncated_command[BUF_SIZE - 47];
                  strncpy(truncated_command, command, BUF_SIZE - 47);
                  snprintf(message, BUF_SIZE, "%sError: no command named %s%s%s !%s", ERROR_COLOR, BYELLOW, truncated_command, ERROR_COLOR, RESET);
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
