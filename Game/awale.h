#ifndef GAME_H
#define GAME_H

typedef struct
{
    char * name;
    unsigned int score = 0;
} Player;

typedef struct
{
    Player players[2];
    unsigned int board[12] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    unsigned int turn = 0;
} Game;

#endif /* guard */