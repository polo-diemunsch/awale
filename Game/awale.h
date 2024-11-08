#ifndef GAME_H
#define GAME_H

typedef struct
{
    char * name;
    unsigned int score;
} Player;

typedef struct
{
    Player players[2];
    unsigned int board[12];
    unsigned int round;
    unsigned int turn;
    unsigned int direction; //clockwise=0, anti-clockwise=1
} Game;

int slot_belongs_to_player (int player, int slot);
Game * create_game(char * player1_name, char * player2_name, int direction);
int execute_round(Game * game, int player, int slot);
void print_game (Game * game);

#endif /* guard */