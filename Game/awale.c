#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "awale.h"

Game * createGame(char * player1Name, char * player2Name) {
    Game * game = malloc(sizeof(Game));
    Player player1;
    player1.name = player1Name;
    Player player2;
    player2.name = player2Name;
}
