#include <stdio.h>
#include <stdbool.h>
#include "awale.h"

int main (){
    Game * game = create_game("john","doe",0);
    

// Continue rounds until one player has no seeds left
int round = 13;
// Check if one side is empty to end the game
bool john_empty = true;
bool doe_empty = true;
while (true) {

    for (int i = 0; i < 6; ++i) {
        if (game->board[i] > 0) john_empty = false;
        if (game->board[6+i] > 0) doe_empty = false;
    }

    if (john_empty || doe_empty) break;

    // John’s turn (try from left to right)
    for (int i = 0; i < 6; ++i) {
        if (game->board[i] > 0) {
            execute_round(game, 0, i);
            print_game(game);
            break;
        }
    }

    // Check again if one side is empty
    john_empty = true;
    doe_empty = true;

    for (int i = 0; i < 6; ++i) {
        if (game->board[i] > 0) john_empty = false;
        if (game->board[6+i] > 0) doe_empty = false;
    }

    if (john_empty || doe_empty) break;

    // Doe’s turn (try from left to right)
    for (int i = 6; i < 12; ++i) {
        if (game->board[i] > 0) {
            execute_round(game, 1, i);
            print_game(game);
            break;
        }
    }
}

execute_round(game, 1, 10); //6 should not be allowed, should feed john
print_game(game);


}