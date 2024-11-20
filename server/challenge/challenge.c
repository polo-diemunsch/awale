#include <string.h>

#include "challenge.h"
#include "../communication/communication.h"
#include "../../lib/console.h"

int challenge(Client *challenger, Client *challengee, Challenge *challenges, int *actual)
{
   char message[BUF_SIZE - 1];

   if (challenger == challengee)
   {
      send_message_to_client(challenger, FRIENDS);
      return -1;
   }
   if (challenger->game != NULL)
   {
      snprintf(message, BUF_SIZE - 1, "%sYou can't challenge when you are already playing.%s", INFORMATION_COLOR, RESET);
      send_message_to_client(challenger, message);
      return -1;
   }
   if (challengee->game != NULL)
   {
      snprintf(message, BUF_SIZE - 1, "%s%s%s is already playing and can't accept your challenge. Try again later.%s", OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
      send_message_to_client(challenger, message);
      return -1;
   }

   int i;

   // accept challenge
   for (i = 0; i < *actual; i++)
   {
      if (challengee == challenges[i].challenger && challenger == challenges[i].challengee)
      {
         snprintf(message, BUF_SIZE - 1, "%s%s%s accepted your challenge!%s", OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
         send_message_to_client(challengee, message);

         // snprintf(message, BUF_SIZE - 1, "%sChallenge accepted!%s", INFORMATION_COLOR, RESET);
         // send_message_to_client(challenger, message);

         remove_challenge(challenges, i, actual);

         return 1;
      }
   }

   // challenge already exists
   for (i = 0; i < *actual; i++)
   {
      if (challenger == challenges[i].challenger && challengee == challenges[i].challengee)
      {
         snprintf(message, BUF_SIZE - 1, "%sYou already sent a challenge to %s%s%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, RESET);
         send_message_to_client(challenger, message);

         return -1;
      }
   }

   // create challenge
   challenges[*actual].challenger = challenger;
   challenges[*actual].challengee = challengee;
   (*actual)++;

   snprintf(message, BUF_SIZE - 1, "%sYou were challenged by %s%s%s. Use %schallenge %s%s to accept.%s", INFORMATION_COLOR, OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
   send_message_to_client(challengee, message);

   snprintf(message, BUF_SIZE - 1, "%sYou challenged %s%s%s.%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
   send_message_to_client(challenger, message);

   return 0;
}

int decline_challenge(Client *challenger, Client *challengee, Challenge *challenges, int *actual)
{
   char message[BUF_SIZE - 1];

   if (challenger == challengee)
   {
      send_message_to_client(challenger, FRIENDS);
      return -1;
   }

   int i;

   // decline challenge
   for (i = 0; i < *actual; i++)
   {
      if (challengee == challenges[i].challenger && challenger == challenges[i].challengee)
      {
         snprintf(message, BUF_SIZE - 1, "%s%s%s declined your challenge!%s", OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
         send_message_to_client(challengee, message);

         snprintf(message, BUF_SIZE - 1, "%sYou declined %s%s%s challenge!%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
         send_message_to_client(challenger, message);

         remove_challenge(challenges, i, actual);

         return 1;
      }
   }

   // cancel challenge
   for (i = 0; i < *actual; i++)
   {
      if (challenger == challenges[i].challenger && challengee == challenges[i].challengee)
      {
         snprintf(message, BUF_SIZE - 1, "%sYou canceled the challenge sent to %s%s%s. (coward)%s", INFORMATION_COLOR, OPPONENT_COLOR, challengee->name, INFORMATION_COLOR, RESET);
         send_message_to_client(challenger, message);

         snprintf(message, BUF_SIZE - 1, "%s%s%s canceled the challenge they sent you.%s", OPPONENT_COLOR, challenger->name, INFORMATION_COLOR, RESET);
         send_message_to_client(challengee, message);

         remove_challenge(challenges, i, actual);

         return 1;
      }
   }

   snprintf(message, BUF_SIZE - 1, "%sError: You did not challenge %s%s%s or they did not challenge you. You can do that using the %schallenge%s command.%s", ERROR_COLOR, OPPONENT_COLOR, challengee->name, ERROR_COLOR, INFORMATION_COLOR, ERROR_COLOR, RESET);
   send_message_to_client(challenger, message);

   return -1;
}

void remove_challenge(Challenge *challenges, int to_remove, int *actual)
{
   /* we remove the challenge in the array */
   memmove(challenges + to_remove, challenges + to_remove + 1, (*actual - to_remove - 1) * sizeof(Challenge));
   (*actual)--;
}
