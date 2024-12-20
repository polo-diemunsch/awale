#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "client.h"
#include "display.h"
#include "../serialization/serialization.h"
#include "../game/awale.h"
#include "../lib/math.h"
#include "../lib/console.h"

static volatile sig_atomic_t terminal_resized = 0;

/* signal handler for SIGWINCH (terminal resize) */
static void handle_sigwinch(int sig) {
   terminal_resized = 1;
}

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

static void app(const char *address, const char *name, const char * password)
{
   SOCKET sock = init_connection(address);
   char buffer[BUF_SIZE];

   Messages *messages = malloc(sizeof(Messages));
   for (int i = 0; i < MESSAGES_COUNT; i++)
      messages->messages[i] = NULL;
   messages->oldest_message_index = 0;
   messages->newest_message_index = 0;
   messages->oldest_displayed_message_index = 0;
   messages->newest_displayed_message_index = 0;

   Game *game = NULL;
   unsigned char who_am_i = 0;

   fd_set rdfs;
   struct winsize ws;
   if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
   {
      perror("ioctl()");
      exit(errno);
   }

   /* send our name */
   char * name_and_password = malloc(strlen(name)+1+strlen(password)+1);
   memcpy(name_and_password,name,strlen(name));
   name_and_password[strlen(name)]='|';
   memcpy(name_and_password+strlen(name)+1,password,strlen(password));
   name_and_password[strlen(name)+1+strlen(password)]='\0';
   printf("Here goes %s",name_and_password);
   write_server(sock, name_and_password);

   /* set up signal handler for SIGWINCH */
   signal(SIGWINCH, handle_sigwinch);

   update_interface(ws, messages, game, who_am_i);

   while (1)
   {
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the socket */
      FD_SET(sock, &rdfs);

      if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1 && terminal_resized == 0)
      {
         perror("select()");
         exit(errno);
      }

      /* check if the terminal has been resized */
      if (terminal_resized) {
         /* get the new terminal size */
         if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
            update_interface(ws, messages, game, who_am_i);
         }
         terminal_resized = 0;
         continue;
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs))
      {
         fgets(buffer, BUF_SIZE - 2, stdin);
         {
            char *p = NULL;
            p = strstr(buffer, "\n");
            if (p != NULL)
            {
               *p = 0;
            }
            else
            {
               /* fclean */
               buffer[BUF_SIZE - 1] = 0;
            }
         }
         if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "e") == 0)
         {
            break;
         }
         else if (strcmp(buffer, "home") == 0 || strcmp(buffer, "h") == 0)
         {
            if (game != NULL)
            {
               free(game);
               game = NULL;
            }
         }
         write_server(sock, buffer);
      }
      /* something from server */
      else if (FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /* server down */
         if(n == 0)
         {
            printf("Server disconnected !\n");
            break;
         }

         CommunicationType type = *buffer;
         switch (type)
         {
            case MESSAGE:
               messages->newest_message_index = mod(messages->newest_message_index + 1, MESSAGES_COUNT);
               read_string_from_buffer(&messages->messages[messages->newest_message_index], buffer + 1);
               if (messages->newest_message_index == messages->oldest_message_index)
                  messages->oldest_message_index = mod(messages->oldest_message_index + 1, MESSAGES_COUNT);
               break;

            case GAME_INIT:
               if (game != NULL)
                  free(game);
               game = malloc(sizeof(Game));
               game->players[0].name = NULL;
               game->players[1].name = NULL;
               who_am_i = unserialize_game(game, buffer + 1);
               break;

            case GAME_STATE:
               if (game != NULL)
                  unserialize_game_state(game, buffer + 1);
               break;

            case GAME_END:
               if (game != NULL)
               {
                  free(game);
                  game = NULL;
               }
               messages->newest_message_index = mod(messages->newest_message_index + 1, MESSAGES_COUNT);
               read_string_from_buffer(&messages->messages[messages->newest_message_index], buffer + 1);
               if (messages->newest_message_index == messages->oldest_message_index)
                  messages->oldest_message_index = mod(messages->oldest_message_index + 1, MESSAGES_COUNT);
               break;

            default:
               break;
         }
      }

      update_interface(ws, messages, game, who_am_i);
   }

   end_connection(sock);

   for (int i = 0; i < MESSAGES_COUNT; i++)
   {
      if (messages->messages[i] != NULL)
         free(messages->messages[i]);
   }
   free(messages);

   if (game != NULL)
      free(game);

   clear_screen();
}

static int init_connection(const char *address)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };
   struct hostent *hostinfo;

   if (sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }

   hostinfo = gethostbyname(address);
   if (hostinfo == NULL)
   {
      fprintf (stderr, "Unknown host %s.\n", address);
      exit(EXIT_FAILURE);
   }

   sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr_list[0];
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if (connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
   {
      perror("connect()");
      exit(errno);
   }

   return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if ((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;
}

static void write_server(SOCKET sock, const char *buffer)
{
   if (send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv)
{
   printf("%d",argc);
   if (argc < 4)
   {
      printf("Usage : %s [address] [pseudo] [password]\n", argv[0]);
      return EXIT_FAILURE;
   }

   init();

   app(argv[1],argv[2],argv[3]);

   end();

   return EXIT_SUCCESS;
}
