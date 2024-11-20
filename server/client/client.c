#include <string.h>

#include "client.h"
#include "../../lib/console.h"

Client *find_client_by_name(Client *clients, int actual, const char *name, char *error)
{
   int i;
   for (i = 0; i < actual; i++)
   {
      if (strcmp(name, clients[i].name) == 0)
         return &clients[i];
   }

   char player_name[BUF_SIZE - 47];
   strncpy(player_name, name, BUF_SIZE - 47);
   snprintf(error, BUF_SIZE - 1, "%sError: player %s%s%s not found%s", ERROR_COLOR, OPPONENT_COLOR, player_name, ERROR_COLOR, RESET);
   return NULL;
}

void remove_client(Client *clients, int to_remove, int *actual)
{
   /* we remove the client in the array */
   memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*actual)--;
}

void clear_clients(Client *clients, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      closesocket(clients[i].sock);
   }
}
