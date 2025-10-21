#include "../include/GameLogic.h"
#include <iostream>

int main()
{
    std::cout << std::boolalpha;
    GameLogic game;
    game.printBoard();
    game.movePiece(RIGHT);
    game.printBoard();
    game.movePiece(LEFT);
    game.printBoard();
    game.DASLeft();
    game.printBoard();
    return 0;
}