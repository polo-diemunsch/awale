#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>


#include "awale.h"

//check functions
int slot_belongs_to_player(int player, int slot) {
    if (slot < 0 || slot >= 12) {
        printf("Slot is out of range!\n");
        return -1;
    }
    return (player == 0 && slot < 6) || (player == 1 && slot >= 6);
}

int player_is_broke (int player, game * game) {
    for (int i=0;i<12;++i){
        if(slot_belongs_to_player(player,i)) {
            if(game->board[slot]!=0){
                return 0
            }
        }
    }
    return 1;
}




Game * create_game(char * player0_name, char * player1_name, int direction) {
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
    if (game == NULL) {
        perror("Failed to allocate memory for game");
        return NULL;
    }
    *game = (Game){
        .players = {player0, player1},
        .board = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},
        .round = 1,
        .turn = 0, //rand() % 2,
        .direction = direction
    };
    return game;
}

int execute_round(Game * game, int player, int slot) {
    //check que le slot est bien du côté du player ? 
    if (game->turn != player) {
        printf("Wrong player!\n");
        return -1;
    }
    
    if (slot_belongs_to_player(player, slot) != 1) {
        printf("Slot does not belong to player!\n");
        return -1;
    }

    int seeds=game->board[slot];
    game->board[slot]=0; //empty slot where seeds were taken


    int current_slot=slot;
    for (int i=1;i<=seeds;++i) {
        current_slot = (current_slot+1)%12; //slot in which we are about to add a seed
        if(current_slot==slot){ //except if it is the one we took the seeds in
            i--;
            continue;
        }
        game->board[current_slot]++; //seed added
    }

    while ((game->board[current_slot]==2 || game->board[current_slot]==3) && !slot_belongs_to_player(player,current_slot)){ //check if seeds are gained
        game->players[player].score+=game->board[current_slot]; //increment score
        game->board[current_slot]=0; //empty slot
        current_slot --;
    }

    if(game->players[player].score>24) {
        printf("%s is the winner!",game->players[player]);
        return 1;
    }
    //if everything was allowed
    game->turn=(game->turn+1)%2;
    game->round++;
    return 0;
}



void print_game (Game * game) { 
    printf("Players : %s and %s \n",game->players[0].name, game->players[1].name);
    printf("Round : %d", game->round);
    printf("     Turn : %s \n",game->players[game->turn].name);
    printf("Board : \n");
    for(int i=11;i>5;--i){
        printf("%d ",game->board[i]);
    }
    printf("\n");
    for(int i=0;i<6;++i){
        printf("%d ",game->board[i]);
    }
    printf("\n");
    printf("Scores :     %s: %d      %s: %d\n",game->players[0].name,game->players[0].score,game->players[1].name,game->players[1].score);
    printf("\n\n");
}
            

