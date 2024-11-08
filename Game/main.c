#include <stdio.h>
#include "awale.h"

int main (){
    Game * game = create_game("john","doe",0);
    
    //init
    print_game(game);
    //round 1
    execute_round(game,0,0);
    print_game(game);


    //round 2 fails because not the right player
    execute_round(game,0,0);
    print_game(game);

    //round 2bis fails because not the right slot
    execute_round(game,1,0);
    print_game(game);

    //round 2ter
    execute_round(game,1,7);
    print_game(game);

    //round 3
    execute_round(game,0,3);
    print_game(game);

    //round 4
    execute_round(game,1,6);
    print_game(game);


    //round 4
    execute_round(game,0,2);
    print_game(game);

}