#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include "../communication/communication.h"

typedef struct {
   SOCKET sock;
   char name[BUF_SIZE];
} Client;

#endif /* guard */
