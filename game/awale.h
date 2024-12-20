#ifndef AWALE_H
#define AWALE_H

#define BOARD_SIZE 12

typedef struct
{
    char *name;
    unsigned char score;
} Player;

typedef struct
{
    Player players[2];
    char board[BOARD_SIZE];
    unsigned char round;
    unsigned char turn;
    unsigned char direction;
    unsigned char history[160];
    unsigned char nb_identical_rounds;
    unsigned char winner;
    char name[400];
} Game;

int slot_belongs_to_player(unsigned char player, unsigned char slot);
int player_is_broke(Game *game, unsigned char player);
unsigned char distribute_seeds(Game *game, unsigned char slot);
void collect_seeds(Game *game, unsigned char player, unsigned char current_slot);
unsigned char is_feeding_move(Game *game, unsigned char player, unsigned char slot);
unsigned char is_starving_move(Game *game, unsigned char player, unsigned char current_slot);
unsigned char exists_move_to_feed(Game *game, unsigned char player);
Game *create_game(char *player0_name, char *player1_name, unsigned char direction);
int execute_round(Game *game, unsigned char player, unsigned char slot, char *error);
void print_game(Game *game);

#endif 