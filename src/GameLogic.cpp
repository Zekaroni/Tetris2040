#include "../include/GameLogic.h"
#include <iostream>

/*
This code is for the queue generation to be used for the bag.
NOTE: May add different bag generation methods in the future
*/
PieceGenerator::PieceGenerator() : rng(std::chrono::system_clock::now().time_since_epoch().count())
{
    fillQueue();
};

uint8_t PieceGenerator::getNextPiece()
{
    if (queue.size() < GAME_CONSTANTS::BAG_MIN_LIMIT)
    { // Replenish the bag when down to (TO WHAT? :'( )
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
    // Set all def values
    currentPiece = Piece();
    score = 0;
    level = 0;
    playfield.reset();
    gamestate = GameState();

    for (int i = 0; i < GAME_CONSTANTS::TILE_COUNT; ++i)
    {
        tileData[i] = TileAttributes();
    }

    generateNewPiece();
};

void GameLogic::update()
{
    placePiece();
    generateNewPiece();
};

bool GameLogic::isGameOver()
{
    return false;
};

bool GameLogic::movePieceIfValid(Direction dir)
{// NOTE: I just realised that for the movemnet I can use the mod operator. You'll know
 // NOTE: yeah, no I don't know what I was cooking, rip
 // NOTE: looked again and yeah idk, I feel like I was cooking but idk
 // NOTE: Still have no idea how I'd use the mod operator
    return isValidPosition(dir);
};

bool GameLogic::movePiece(Direction dir)
{
    switch (dir)
    {
        case RIGHT:
            currentPiece.position++;
            return true;
        case LEFT:
            currentPiece.position--;
            return true;
        case DOWN:
            currentPiece.position += GAME_CONSTANTS::BOARD_WIDTH;
            return true;
        default:
            return false;
    };
}

bool GameLogic::revertPiece(Direction dir)
{
    switch (dir)
    {
        case RIGHT:
            currentPiece.position--;
            return true;
        case LEFT:
            currentPiece.position++;
            return true;
        case DOWN:
            currentPiece.position -= GAME_CONSTANTS::BOARD_WIDTH;
            return true;
        default:
            return false;
    };
}

void GameLogic::DAS(Direction dir)
{ // NOTE: DAS right and left have to take into account the fact pieces could be alraedy placed
    while (isValidPosition(dir)){};
};

void GameLogic::rotatePiece(Rotation dir)
{
    currentPiece.rotation = currentPiece.rotation + dir % GAME_CONSTANTS::ROTATION_COUNT;
};

void GameLogic::softDrop()
{
    isValidPosition(DOWN);
};

void GameLogic::hardDrop()
{
    while(isValidPosition(DOWN)){};
};

void GameLogic::placePiece()
{
    const std::bitset<16> pieceShape(GameData::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation]);
    int startingRow = currentPiece.position / GAME_CONSTANTS::BOARD_WIDTH;
    int startingCol = currentPiece.position % GAME_CONSTANTS::BOARD_WIDTH;
    for (int row = 0; row < (GAME_CONSTANTS::PIECE_SIZE); row++)
    {
        for (int col = 0; col < (GAME_CONSTANTS::PIECE_SIZE); col++)
        {
            uint8_t pieceBitIndex = 15 - (row * GAME_CONSTANTS::PIECE_SIZE + col);
            if(pieceShape[pieceBitIndex])
            {
                playfield[((startingRow+row) * GAME_CONSTANTS::BOARD_WIDTH) + (startingCol + col)] = 1;
            }
        }
    }
};

uint64_t GameLogic::getScore() const
{
    return score;
};

uint8_t GameLogic::getLevel() const
{
    return level;
};

void GameLogic::generateNewPiece()
{
    currentPiece.pieceIndex = pieceRandomizer.getNextPiece();
    currentPiece.position = GAME_CONSTANTS::STARTING_POSITION;
    currentPiece.rotation = GAME_CONSTANTS::STARTING_ROTATION;
    currentPiece.isTouchingDown = false;
    // touchdown time shouldn't need changed because of isTouchingDown implementation
};

uint16_t GameLogic::getRow(uint8_t rowIndex)
{
    return (playfield >> (rowIndex * GAME_CONSTANTS::BOARD_WIDTH)).to_ulong() & GAME_CONSTANTS::FULL_LINE_MASK;
};

bool GameLogic::isValidPosition(Direction dir)
{
    movePiece(dir);

    const std::bitset<16> pieceShape(GameData::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation]);
    
    uint8_t baseRow    = currentPiece.position /  GAME_CONSTANTS::BOARD_WIDTH;
    uint8_t baseCol    = currentPiece.position %  GAME_CONSTANTS::BOARD_WIDTH;
    
    // std::cout
    // << "Base column: "         << (int)baseCol << std::endl
    // << "Base row: "            << (int)baseRow << std::endl;
    
    // If moving left and the mask is on the wall, it moves the piece to the right
    // if there is no piece there currently
    if (baseCol == 0 && !dir)
    {
        std::cout << "Running first column check" << std::endl;
        for (uint8_t row = 0; row < GAME_CONSTANTS::PIECE_SIZE; row++)
        {
            if (pieceShape[row * GAME_CONSTANTS::PIECE_SIZE] ||
               (playfield[currentPiece.position + (row * GAME_CONSTANTS::BOARD_WIDTH)] && pieceShape[row * GAME_CONSTANTS::PIECE_SIZE + 1])
            ) // TODO: Test this part. I think it is correct, but have not tested it.
            {
                return false;
            }
        }
        currentPiece.position--;
        return true;
    };
    
    for (uint8_t row = 0; row < GAME_CONSTANTS::PIECE_SIZE; ++row)
    {
        for (uint8_t col = 0; col < GAME_CONSTANTS::PIECE_SIZE; ++col)
        {
            uint8_t pieceBitIndex = 15 - (row * GAME_CONSTANTS::PIECE_SIZE + col);
            if (pieceShape[pieceBitIndex]) 
            {
                uint8_t targetCol = baseCol + col;
                uint8_t targetRow = baseRow + row;
                
                int boardIndex = targetRow * GAME_CONSTANTS::BOARD_WIDTH + targetCol;
                
                if (
                    (targetCol >= GAME_CONSTANTS::BOARD_WIDTH) ||
                    (targetRow >= GAME_CONSTANTS::BOARD_HEIGHT) ||
                    (playfield[boardIndex])
                )
                {
                    std::cout
                    << "Column when collide: " << (int)targetCol << std::endl
                    << "Row when collide: "    << (int)targetRow << std::endl
                    << std::endl;
                    revertPiece(dir);
                    return false;
                } 
            }
        }
    }
    
    return true; 
};

void GameLogic::updateRows(uint8_t startRow, uint8_t rowCount)
{
    std::bitset<GAME_CONSTANTS::TILE_COUNT> upperMask = playfield >>
        ((GAME_CONSTANTS::BOARD_HEIGHT - startRow - 1) * GAME_CONSTANTS::BOARD_WIDTH);
    
    std::bitset<GAME_CONSTANTS::TILE_COUNT> lowerMask = GAME_CONSTANTS::FULL_BOARD_MASK >>
        ((startRow + rowCount) * GAME_CONSTANTS::BOARD_WIDTH); // (+ 1) ?

    playfield = (
        (playfield & lowerMask) |
        (upperMask << 
            ((GAME_CONSTANTS::BOARD_HEIGHT - startRow - rowCount - 1) * GAME_CONSTANTS::BOARD_WIDTH)
        )
    );
};

bool GameLogic::checkAndClearLines()
{
    uint8_t linesCleared    = 0;
    uint8_t concurrentLines = 0;
    uint8_t currentRow      = GAME_CONSTANTS::BOARD_HEIGHT - 1;
    
    while (currentRow < GAME_CONSTANTS::BOARD_HEIGHT) // uses underflow
    {
        if (getRow(currentRow) == GAME_CONSTANTS::FULL_LINE_MASK)
        {
            ++concurrentLines;
        }
        else if (concurrentLines)
        {
            updateRows(currentRow, concurrentLines);
            linesCleared += concurrentLines;
            currentRow   += concurrentLines;
            concurrentLines = 0;
        }
        --currentRow;
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
    uint8_t index = linesCleared + (gamestate.fullByte & 1100); // TODO: Fix
    score += GAME_CONSTANTS::SCORE_LOOKUP_TABLE[linesCleared];
};

std::bitset<GAME_CONSTANTS::TILE_COUNT> GameLogic::getPlayfield() const
{
    return playfield;
};

void GameLogic::printBoard() const
{ // WARNING: Remove before "real" compiles
    std::bitset<GAME_CONSTANTS::TILE_COUNT> tempBoard = playfield;
    const std::bitset<16> pieceShape(GameData::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation]);
    
    uint8_t rowIndex    = currentPiece.position /  GAME_CONSTANTS::BOARD_WIDTH;
    uint8_t columnIndex = currentPiece.position - (rowIndex * GAME_CONSTANTS::BOARD_WIDTH);

    for (uint8_t row = 0; row < GAME_CONSTANTS::PIECE_SIZE; ++row)
    {
        for (uint8_t column = 0; column < GAME_CONSTANTS::PIECE_SIZE; ++column)
        {
            int pieceBitIndex = 15 - (row * GAME_CONSTANTS::PIECE_SIZE + column);
            if (pieceShape[pieceBitIndex])
            {
                uint16_t boardIndex = ((row + rowIndex) * GAME_CONSTANTS::BOARD_WIDTH) + (column + columnIndex);
                if (boardIndex < GAME_CONSTANTS::TILE_COUNT)
                {
                    tempBoard.set(boardIndex);
                }
            }
        }
    }

    for (int r = 0; r < GAME_CONSTANTS::BOARD_HEIGHT; ++r)
    {
        for (int c = 0; c < GAME_CONSTANTS::BOARD_WIDTH; ++c)
        {
            int index = r * GAME_CONSTANTS::BOARD_WIDTH + c;
            std::cout << (tempBoard[index] ? "#" : ".");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
};