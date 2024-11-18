#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "awale.h"
#include "../communication/communication.h"

#define FALSE 0
#define TRUE 1
#define BUF_SIZE 1024

//useful functions
int slot_belongs_to_player(unsigned char player, unsigned char slot)
{
    //returns TRUE if slot belongs to player, FALSE if it doesn't and -1 if out of range
    if (slot < 0 || slot >= BOARD_SIZE)
    {
        printf("Slot is out of range!\n");
        return -1;
    }
    return (player == 0 && slot < BOARD_SIZE / 2) || (player == 1 && slot >= BOARD_SIZE / 2);
}

int player_is_broke(Game *game, unsigned char player)
{
    //returns 1 if player is broke, 0 otherwise
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if (slot_belongs_to_player(player,i))
        {
            if (game->board[i] != 0)
                return 0;
        }
    }
    return 1;
}

char distribute_seeds(Game *game, unsigned char slot)
{
   int seeds = game->board[slot];
    game->board[slot]=0; //empty slot where seeds were taken

    unsigned char current_slot = slot;
    for (int i = 1; i <= seeds; ++i)
    {
        current_slot = (current_slot + 1) % BOARD_SIZE; //slot in which we are about to add a seed
        if(current_slot == slot) //except if it is the one we took the seeds in
        {
            i--;
            continue;
        }
        game->board[current_slot]++; //seed added
    }
    return current_slot;
}

void collect_seeds(Game *game, unsigned char player, unsigned char current_slot)
{
    while ((game->board[current_slot] == 2 || game->board[current_slot] == 3) && !slot_belongs_to_player(player, current_slot))
    { 
        game->players[player].score += game->board[current_slot]; //increment score
        game->board[current_slot] = 0; //empty slot
        current_slot--;
    }
}

char is_feeding_move(Game *game, unsigned char player, unsigned char slot)
{
    Game *game_copy = malloc(sizeof(Game));
    *game_copy = *game;
    char other_player = (player + 1) % 2;
    if(slot_belongs_to_player(player, slot))
    {
        distribute_seeds(game_copy, slot);
        if(!player_is_broke(game_copy, other_player))
        {
            free(game_copy);
            return 1;
        }
    }
    free(game_copy);
    return 0;
}

char is_starving_move(Game *game, unsigned char player, unsigned char current_slot)
{
    int other_player = (player + 1) % 2;
    Game *game_copy = malloc(sizeof(Game));
    *game_copy = *game;
    collect_seeds(game_copy, player, current_slot);
    if (player_is_broke(game_copy, other_player)){
        free(game_copy);
        return 1;
    }
    free(game_copy);
    return 0;
}

char exists_move_to_feed(Game *game, unsigned char player)
{ 
    //returns 1 if there exists a move to feed the other player
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if(is_feeding_move(game, player, i))
            return 1;
    }
    return 0;
}


Game *create_game(char *player0_name, char *player1_name, unsigned char direction)
{
    srand(time(NULL));
    Player player0 = {
        .name = player0_name,
        .score = 0,
    };

    Player player1 = {
        .name = player1_name,
        .score = 0,
    };

    Game *game = malloc(sizeof(Game));
    if (game == NULL)
    {
        perror("Failed to allocate memory for game");
        return NULL;
    }
    *game = (Game) {
        .players = {player0, player1},
        .board = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
        .round = 0,
        .turn = rand() % 2,
        .direction = direction,
    };
    //to fill the name
    time_t now = time(NULL);         
    struct tm *lt = localtime(&now);  
    char datetime_str[30];
    if (lt != NULL) {
        strftime(datetime_str, sizeof(datetime_str), "%Y-%m-%d_%H-%M-%S", lt);
    }else{
        printf("Error in game creation");
        free(game);
        return NULL;
    }
        snprintf((char *)game->name, sizeof(game->name), "%s_%s_%s", player0_name, player1_name, datetime_str);
    //save in a file
    FILE * f;
    f=fopen((char *)game->name,"a");
    char *game_init = malloc(BUF_SIZE); 
    if (!game_init) {
        perror("Failed to allocate memory for game_init");
        free(game);
        return NULL;
    }
    serialize_game_init_history(game,game_init); //serialize_game_init(game,(unsigned char)0,game_init);
    fprintf(f,"%s",game_init);
    fclose(f);

    return game;
}

//main function
int execute_round(Game *game, unsigned char player, unsigned char slot)
{
    //returns 0 if goes correctly
    //returns -1 if the move was not valid
    //returns 1 if game over
    if (game->turn != player)
    {
        printf("Wrong player!\n");
        return -1;
    }
    
    if (slot_belongs_to_player(player, slot) != 1)
    {
        printf("Slot does not belong to player!\n");
        return -1;
    }

    if (game->board[slot] == 0)
    {
        printf("Slot is empty!");
        return -1;
    }

    //check if the move feeds the other player if necessary
    char other_player = ((player + 1) % 2);
    if(player_is_broke(game, other_player))
    {
        printf("Other player is broke, you have to feed him\n");
        if(!is_feeding_move(game, player, slot) && exists_move_to_feed(game, player))
        {
            printf("You have to select a move that feeds the other player!");
            return -1;
        }
        else
        {
            //increment score (all seeds go to the player)
            for (int i = 0; i < BOARD_SIZE; ++i)
            {
                game->players[player].score += game->board[i];
                game->board[i] = 0;
                return 1;
            }
            printf("Game over! %s won",game->players[player].name);
        }
    }

    unsigned char current_slot = distribute_seeds(game, slot);

    //check if seeds are gained
    if ((game->board[current_slot] == 2 || game->board[current_slot] == 3) && !slot_belongs_to_player(player, current_slot))
    {
        if(!is_starving_move(game, player, slot))
            collect_seeds(game, player, current_slot);
    }

    //update history
    for(int i=0;i<12;++i){
        game->history[game->round*12+i]=game->board[i];
    }
    for(int i=0;i<12;++i){
        if(game->history[game->round*12+i]!=game->history[game->round*11+i]){
            //int nb_id_rounds = 0; 
            //memcpy(game->nb_identical_rounds, &nb_id_rounds, sizeof(nb_id_rounds));
            game->nb_identical_rounds=0;
            break;
            }else{
                game->nb_identical_rounds++;
            }
        
    }

    //check if a player won with their score
    if(game->players[player].score > 24)
    {
        printf("%s is the winner!", game->players[player].name);
        game->winner=player;
        return 1;
    }
    
    game->turn = (game->turn + 1) % 2;
    game->round++;
    return 0;

    //check if the rounds have been identical for 30 rounds
    if(game->nb_identical_rounds>29){
        game->winner=(game->players[0].score > game->players[2].score) ? 0 : 1;
        printf("Game is stuck.%c won.\n",game->winner);
        
    }

    //history save
    FILE * f;
    f=fopen((char *)game->name,"a");
    fprintf(f,"%d",itoa(slot)); 
    fclose(f);
}

void print_game(Game *game)
{ 
    printf("Players : %s and %s \n",game->players[0].name, game->players[1].name);
    printf("Round : %d", game->round);
    printf("     Turn : %s \n",game->players[game->turn].name);
    printf("Board : \n");
    for(int i=11;i>5;--i)
        printf("%d ",game->board[i]);

    printf("\n");
    for(int i=0;i<6;++i)
        printf("%d ",game->board[i]);

    printf("\n");
    printf("Scores :     %s: %d      %s: %d\n",game->players[0].name,game->players[0].score,game->players[1].name,game->players[1].score);
    printf("\n\n");
}


            

