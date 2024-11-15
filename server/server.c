#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server.h"
#include "client.h"
#include "../lib/console.h"

static void init(void)
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

static void end(void)
{
#ifdef WIN32
   WSACleanup();
#endif
}

static void app(void)
{
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   /* the index for the array */
   int actual = 0;
   int max = sock;
   /* an array for all clients */
   Client clients[MAX_CLIENTS];

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
         strncpy(c.name, buffer, BUF_SIZE - 1);
         clients[actual] = c;
         actual++;

         char message[BUF_SIZE - 1];
         char player_name[BUF_SIZE - 43];
         strncpy(player_name, c.name, BUF_SIZE - 43);
         snprintf(message, BUF_SIZE, "%sBienvenue sur Awalé %s%s%s !%s", BYELLOW, OWN_COLOR, player_name, BYELLOW, RESET);
         send_message_to_client(c, message);
      }
      else
      {
         int i = 0;
         for(i = 0; i < actual; i++)
         {
            /* a client is talking */
            if(FD_ISSET(clients[i].sock, &rdfs))
            {
               Client client = clients[i];
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

               char buffer_copy[BUF_SIZE], *command, *remainder;
               strcpy(buffer_copy, buffer);

               command = strtok_r(buffer_copy, " ", &remainder);

               if (strcmp(command, "send") == 0)
               {
                  char *target = strtok_r(NULL, " ", &remainder);
                  send_message(clients, client, actual, target, remainder);
               }
               else
               {
                  char message[BUF_SIZE];
                  char truncated_command[BUF_SIZE - 46];
                  strncpy(truncated_command, command, BUF_SIZE - 46);
                  snprintf(message, BUF_SIZE, "%sError: no command named %s%s%s !%s", ERROR_COLOR, BYELLOW, truncated_command, ERROR_COLOR, RESET);
                  send_message_to_client(client, message);
               }

               printf("%s\n", buffer);
               break;
            }
         }
      }
   }

   clear_clients(clients, actual);
   end_connection(sock);
}

static void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

static void send_message_to_client(Client receiver, char *message)
{
   char buffer[BUF_SIZE];
   *buffer = MESSAGE;
   size_t n = write_string_to_buffer(message, buffer + 1);
   write_client(receiver.sock, buffer, n + 1);
}

static void send_message(Client *clients, Client sender, int actual, const char*target, const char *buffer)
{
   if (strcmp(target, "all") == 0)
   {
      send_message_to_all_clients(clients, sender, actual, buffer);
   }
   else
   {
      int i;
      for (i = 0; i < actual; i++)
      {
         if (strcmp(target, clients[i].name) == 0)
         {
            if (sender.sock != clients[i].sock)
               send_message_from_client_to_client(clients[i], sender, buffer);
            else
               send_message_to_client(sender, "\033[36mhttps://www.santemagazine.fr/psycho-sexo/psycho/10-facons-de-se-faire-des-amis-178690\033[0m");

            return;
         }
      }

      char message[BUF_SIZE - 1];
      char player_name[BUF_SIZE - 47];
      strncpy(player_name, target, BUF_SIZE - 47);
      snprintf(message, BUF_SIZE - 1, "%sError: player %s%s%s not found%s", ERROR_COLOR, OPPONENT_COLOR, player_name, ERROR_COLOR, RESET);
      send_message_to_client(sender, message);
   }
}

static void send_message_from_client_to_client(Client receiver, Client sender, const char *buffer)
{
   char message[BUF_SIZE];
   char player_name[BUF_SIZE - 27];

   strncpy(player_name, sender.name, BUF_SIZE - 27);
   snprintf(message, BUF_SIZE, "%sfrom %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, player_name, BWHITE, buffer, RESET);
   send_message_to_client(receiver, message);

   strncpy(player_name, receiver.name, BUF_SIZE - 27);
   snprintf(message, BUF_SIZE, "%sto %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, player_name, BWHITE, buffer, RESET);
   send_message_to_client(sender, message);
}

static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE - 1];
   char player_name[BUF_SIZE - 12];
   for (i = 0; i < actual; i++)
   {
      strncpy(player_name, sender.name, BUF_SIZE - 12);
      snprintf(message, BUF_SIZE, "%s%s%s: %s", clients[i].sock != sender.sock ? OPPONENT_COLOR : OWN_COLOR, player_name, RESET, buffer);
      send_message_to_client(clients[i], message);
   }
}

static int init_connection(void)
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

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer)
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

static void write_client(SOCKET sock, const char *buffer, size_t n)
{
   if(send(sock, buffer, n, 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   init();

   app();

   end();

   return EXIT_SUCCESS;
}
