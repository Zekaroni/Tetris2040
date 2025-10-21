#include "../include/GameLogic.h"
#include <iostream>

int main()
{
    std::cout << std::boolalpha;
    GameLogic game;
    game.printBoard();
    game.rotatePiece(CLOCKWISE);
    game.printBoard();
    game.DASLeft();
    game.printBoard();
    game.hardDrop();
    game.printBoard();
    game.update();
    game.printBoard();
    return 0;
}