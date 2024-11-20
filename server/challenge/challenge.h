#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "../client/client.h"

typedef struct {
   Client *challenger;
   Client *challengee;
} Challenge;

int challenge(Client *challenger, Client *challengee, Challenge *challenges, int *actual);
int decline_challenge(Client *challenger, Client *challengee, Challenge *challenges, int *actual);

void remove_challenge(Challenge *challenges, int to_remove, int *actual);

#endif /* guard */
