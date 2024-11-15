#ifndef GAME_H
#define GAME_H

typedef struct
{
    char * name;
    char score;
} Player;

typedef struct
{
    Player players[2];
    char board[12];
    char round;
    char turn;
    char direction; //clockwise=0, anti-clockwise=1
} Game;

char slot_belongs_to_player(char player, char slot);
int player_is_broke(Game *game, char player);
char distribute_seeds(Game *game, char slot);
void collect_seeds(Game *game, char player, char current_slot);
char is_feeding_move(Game *game, char player, char slot);
char is_starving_move(Game *game, char player, char current_slot);
char exists_move_to_feed(Game *game, char player);
Game * create_game(char * player1_name, char * player2_name, char direction);
int execute_round(Game * game, char player, char slot);
void print_game (Game * game);

#endif /* guard */