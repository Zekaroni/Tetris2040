#include "../include/GameLogic.h"

int main()
{
    // std::cout << std::boolalpha;
    GameLogic game;
    game.printBoard();
    game.rotatePiece(CLOCKWISE);
    game.printBoard();
    game.DAS(LEFT);
    game.printBoard();
    game.hardDrop();
    game.printBoard();
    // game.update();
    // game.printBoard();
    // game.hardDrop();
    // game.printBoard();
    // game.DAS(LEFT);
    // game.printBoard();
    return 0;
}