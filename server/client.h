#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "../communication/communication.h"

typedef struct {
   SOCKET sock;
   char name[NAME_SIZE];
   Game *game;
} Client;

typedef struct {
   Client *challenger;
   Client *challengee;
} Challenge;

#endif /* guard */
