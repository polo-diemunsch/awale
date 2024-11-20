#ifndef SERVER_H
#define SERVER_H

#include "sock.h"
#include "client/client.h"
#include "../game/awale.h"

void init(void);
void end(void);
void app(void);
int init_connection(void);
void end_connection(int sock);

#endif /* guard */
