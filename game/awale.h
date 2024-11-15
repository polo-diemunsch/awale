#ifndef GAME_H
#define GAME_H

#define BOARD_SIZE 12

typedef struct
{
    char * name;
    unsigned char score;
} Player;

typedef struct
{
    Player players[2];
    char board[BOARD_SIZE];
    unsigned char round;
    unsigned char turn;
    unsigned char direction; //clockwise=0, anti-clockwise=1
} Game;

int slot_belongs_to_player(unsigned char player, unsigned char slot);
int player_is_broke(Game *game, unsigned char player);
char distribute_seeds(Game *game, unsigned char slot);
void collect_seeds(Game *game, unsigned char player, unsigned char current_slot);
char is_feeding_move(Game *game, unsigned char player, unsigned char slot);
char is_starving_move(Game *game, unsigned char player, unsigned char current_slot);
char exists_move_to_feed(Game *game, unsigned char player);
Game *create_game(char * player1_name, char * player2_name, unsigned char direction);
int execute_round(Game * game, unsigned char player, unsigned char slot);
void print_game(Game * game);

#endif /* guard */