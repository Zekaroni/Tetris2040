#include "./include/GameLogic.h"

/*
This code is for the queue generation to be used for the bag.
NOTE: May add different bag generation methods in the future
*/
PieceGenerator::PieceGenerator() : rng(std::random_device{}())
{
    fillQueue();
}

uint8_t PieceGenerator::getNextPiece()
{
    if (queue.size() < 7)
    { // Replenish the bag when down to 
        fillQueue();
    }
    uint8_t next = queue.front();
    queue.pop_front();
    return next;
}

const std::deque<uint8_t>& PieceGenerator::getQueue() const
{
    return queue;
}

void PieceGenerator::fillQueue()
{
    bag = {0, 1, 2, 3, 4, 5, 6};
    std::shuffle(bag.begin(), bag.end(), rng);
    for (const uint8_t& piece : bag)
    {
        queue.push_back(piece);
    }
}



/*
This is the code for the actual game logic itself
*/
GameLogic::GameLogic()
{ // The object will start a new game when it is created
    startNewGame();
}

void GameLogic::startNewGame()
{ // TODO: Add logic to start a new game. Inits and everything

}