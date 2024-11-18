#include <string.h>

#include "commands.h"
#include "../lib/console.h"

void send_message(Client *clients, Client sender, int actual, const char*target, const char *buffer)
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
         if (sender.sock != receiver->sock)
            send_message_from_client_to_client(*receiver, sender, buffer);
         else
            send_message_to_client(sender, "\033[36mhttps://www.santemagazine.fr/psycho-sexo/psycho/10-facons-de-se-faire-des-amis-178690\033[0m");
      }
      else
      {
         send_message_to_client(sender, error);
      }
   }
}

void send_message_from_client_to_client(Client receiver, Client sender, const char *buffer)
{
   char message[BUF_SIZE - 1];

   snprintf(message, BUF_SIZE - 1, "%sfrom %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, sender.name, BWHITE, buffer, RESET);
   send_message_to_client(receiver, message);

   snprintf(message, BUF_SIZE - 1, "%sto %s%s%s: %s%s", BWHITE, OPPONENT_COLOR, receiver.name, BWHITE, buffer, RESET);
   send_message_to_client(sender, message);
}

void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer)
{
   int i = 0;
   char message[BUF_SIZE - 1];
   for (i = 0; i < actual; i++)
   {
      snprintf(message, BUF_SIZE - 1, "%s%s%s: %s", clients[i].sock != sender.sock ? OPPONENT_COLOR : OWN_COLOR, sender.name, RESET, buffer);
      send_message_to_client(clients[i], message);
   }
}

int challenge(Client *challenger, Client *challengee, Challenge (*challenges)[MAX_CLIENTS * MAX_CLIENTS])
{
   char message[BUF_SIZE - 1];

   if (challenger->sock == challengee->sock)
   {
      send_message_to_client(*challenger, "\033[36mhttps://www.santemagazine.fr/psycho-sexo/psycho/10-facons-de-se-faire-des-amis-178690\033[0m");
      return -1;
   }
   if (challenger->game != NULL)
   {
      snprintf(message, BUF_SIZE - 1, "%sYou can't challenge when you are already playing.%s", INFORMATION_COLOR, RESET);
      send_message_to_client(*challenger, message);
      return -1;
   }
   if (challengee->game != NULL)
   {
      snprintf(message, BUF_SIZE - 1, "%s%s%s is already playing and can't accept your challenge. Try again later.%s", OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
      send_message_to_client(*challenger, message);
      return -1;
   }

   int i;

   // accept challenge
   for (i = 0; i < MAX_CLIENTS * MAX_CLIENTS; i++)
   {
      Client *current_challenger = (*challenges)[i].challenger;
      Client *current_challengee = (*challenges)[i].challengee;
      if (current_challenger != NULL && current_challengee != NULL && current_challenger->sock == challengee->sock && current_challengee->sock == challenger->sock)
      {
         snprintf(message, BUF_SIZE - 1, "%s%s%s accepted your challenge!%s", OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
         send_message_to_client(*challengee, message);

         // snprintf(message, BUF_SIZE - 1, "%sChallenge accepted!%s", INFORMATION_COLOR, RESET);
         // send_message_to_client(*challenger, message);

         (*challenges)[i].challenger = NULL;
         (*challenges)[i].challengee = NULL;

         return 1;
      }
   }

   int i_free = -1;
   // challenge already exists
   for (i = 0; i < MAX_CLIENTS * MAX_CLIENTS; i++)
   {
      Client *current_challenger = (*challenges)[i].challenger;
      Client *current_challengee = (*challenges)[i].challengee;
      
      if (current_challenger != NULL && current_challenger->sock == challenger->sock && current_challengee != NULL && current_challengee->sock == challengee->sock)
      {
         snprintf(message, BUF_SIZE - 1, "%sYou already sent a challenge to %s%s%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, RESET);
         send_message_to_client(*challenger, message);

         return -1;
      }

      if (current_challenger == NULL && i_free == -1)
         i_free = i;
   }

   // create challenge
   (*challenges)[i_free].challenger = challenger;
   (*challenges)[i_free].challengee = challengee;

   snprintf(message, BUF_SIZE - 1, "%sYou were challenged by %s%s%s. Use %schallenge %s%s to accept.%s", INFORMATION_COLOR, OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
   send_message_to_client(*challengee, message);

   snprintf(message, BUF_SIZE - 1, "%sYou challenged %s%s%s.%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
   send_message_to_client(*challenger, message);

   return 0;
}
