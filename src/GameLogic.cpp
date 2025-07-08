#include "./include/GameLogic.h"

/*
This code is for the queue generation to be used for the bag.
NOTE: May add different bag generation methods in the future
*/
PieceGenerator::PieceGenerator() : rng(std::random_device{}())
{
    fillQueue();
};

uint8_t PieceGenerator::getNextPiece()
{
    if (queue.size() < CONSTANTS::BAG_MIN_LIMIT)
    { // Replenish the bag when down to 
        fillQueue();
    }
    uint8_t next = queue.front();
    queue.pop_front();
    return next;
};

const std::deque<uint8_t>& PieceGenerator::getQueue() const
{
    return queue;
};

void PieceGenerator::fillQueue()
{
    bag = {0, 1, 2, 3, 4, 5, 6};
    std::shuffle(bag.begin(), bag.end(), rng);
    for (const uint8_t& piece : bag)
    {
        queue.push_back(piece);
    }
};



/*
This is the code for the actual game logic itself
*/
GameLogic::GameLogic()
{ // The object will start a new game when it is created
    startNewGame();
};

void GameLogic::startNewGame()
{
    // currentPiece.pieceIndex = pieceGen
};

void GameLogic::update()
{

};

bool GameLogic::isGameOver()
{
    return false;
};

void GameLogic::movePiece(Direction dir)
{// NOTE: I just realised that for the movemnet I can use the mod operator. You'll know
   
};

void GameLogic::movePiece(Direction dir)
{

};

void GameLogic::rotatePiece(Rotation dir)
{

};

void GameLogic::softDrop()
{

};

void GameLogic::hardDrop()
{

};

uint32_t GameLogic::getScore() const
{

};

uint8_t GameLogic::getLevel() const
{

};

bool GameLogic::isValidPosition()
{

};

void GameLogic::spawnNewPiece()
{
    currentPiece.pieceIndex = pieceRandomizer.getNextPiece();
    currentPiece.position = CONSTANTS::STARTING_POSITION;
    currentPiece.rotation = CONSTANTS::STARTING_ROTATION;
    currentPiece.isTouchingDown = false;
};

void GameLogic::clearLine(uint8_t row)
{

};

uint8_t GameLogic::checkAndClearLines()
{

};

void GameLogic::updateScore(uint8_t linesCleared)
{

};
