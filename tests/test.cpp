#include "../include/GameLogic.h"
#include <iostream>

int main()
{
    std::cout << std::boolalpha;
    GameLogic game;
    game.rotatePiece(CLOCKWISE);
    game.printBoard();
    game.DASLeft();
    game.printBoard();
    return 0;
}