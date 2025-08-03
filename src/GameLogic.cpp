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
    spawnNewPiece();
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
 // NOTE: yeah, no I don't know what I was cooking, rip
   
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
    return score;
};

uint8_t GameLogic::getLevel() const
{
    return level;
};

void GameLogic::spawnNewPiece()
{
    currentPiece.pieceIndex = pieceRandomizer.getNextPiece();
    currentPiece.position = CONSTANTS::STARTING_POSITION;
    currentPiece.rotation = CONSTANTS::STARTING_ROTATION;
    currentPiece.isTouchingDown = false;
    // touchdown time shouldn't need changed because of isTouchingDown implementation
};

std::bitset<CONSTANTS::BOARD_WIDTH> GameLogic::getRow(uint8_t rowIndex)
{
    return std::bitset<CONSTANTS::BOARD_WIDTH>((playfield >> (rowIndex * CONSTANTS::BOARD_WIDTH)).to_ulong());
};

void GameLogic::updateRows(uint8_t endRow, uint8_t rowCount)
{
    std::bitset<CONSTANTS::TILE_COUNT> playfieldBuffer = playfield;
    playfieldBuffer << ((CONSTANTS::BOARD_HEIGHT - endRow) * CONSTANTS::BOARD_WIDTH);
};

bool GameLogic::isValidPosition()
{
    const std::bitset<16>& pieceShape = GameData::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation];

    uint8_t rowIndex    = currentPiece.position /  CONSTANTS::BOARD_WIDTH;
    uint8_t columnIndex = currentPiece.position - rowIndex * CONSTANTS::BOARD_WIDTH; // Saves cpu instructions for extra division
    
    // Decided to add quick check for columns instead of running it in the loop for efficency
    if (
        columnIndex + CONSTANTS::PIECE_SIZE > CONSTANTS::BOARD_WIDTH ||
        rowIndex + CONSTANTS::PIECE_SIZE > CONSTANTS::BOARD_HEIGHT
    ) // TODO: Work this out to be sure
    {
        return false;
    }

    for (uint8_t row = 0; row < CONSTANTS::PIECE_SIZE; ++row)
    {
        for (uint8_t column = 0; column < CONSTANTS::PIECE_SIZE; ++column)
        {
            if (
                pieceShape[row * CONSTANTS::PIECE_SIZE + column] &&
                playfield[((row + rowIndex) * CONSTANTS::BOARD_WIDTH) + (column + columnIndex)]
            )
            {
                return false;
            }
        }
    }
    
    return true;
};

bool GameLogic::checkAndClearLines()
{
    // TODO: Update based on Python code that has been tested. I don't believe this is right currently
    uint8_t linesCleared = 0;
    uint8_t concurrentLines = 0;
    
    for (uint8_t row = 0; row < CONSTANTS::BOARD_HEIGHT; ++row)
    {
        if (getRow(row).all())
        {
            ++linesCleared;
            ++concurrentLines;
        }
        else
        {
            if (concurrentLines)
            {
                updateRows(row, concurrentLines);
            }
        }
    }
    
    if (linesCleared)
    {
        updateScore(linesCleared);
        return true;
    }
    return false;
};

void GameLogic::updateScore(uint8_t linesCleared)
{

};
