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

bool GameLogic::movePiece(Direction dir)
{
    switch (dir)
    {
        case RIGHT:
            currentPiece.position++;
            break;
        case LEFT:
            currentPiece.position--;
            break;
        case DOWN:
            currentPiece.position += GAME_CONSTANTS::BOARD_WIDTH;
            break;
        default:
            return false;
    };
    
    if (!isValidPosition())
    {
        revertPiece(dir);
        return false;
    };

    return true;
};

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
};

void GameLogic::DAS(Direction dir)
{ 
    while(movePiece(dir))
    {
        // std::cout << (int)currentPiece.position << std::endl;
    };
};

bool GameLogic::rotatePiece(Rotation dir)
{
    currentPiece.rotation = currentPiece.rotation + dir % GAME_CONSTANTS::ROTATION_COUNT;
    if (!isValidPosition())
    {
        currentPiece.rotation = currentPiece.rotation - dir;
    };
    return true;
};

bool GameLogic::softDrop()
{
    return movePiece(DOWN);
};

bool GameLogic::hardDrop()
{
    int count = 0;
    while(movePiece(DOWN)){count++;};
    if (count)
    {
        placePiece();
        // std::cout << "Moved piece down " << count << " times\n";
        return true;
    };
    return false;
};

void GameLogic::placePiece()
{ // TODO: Make a new method for this functionality
    const std::bitset<16> pieceShape(GAME_DATA::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation]);
    int startingRow = currentPiece.position / GAME_CONSTANTS::BOARD_WIDTH;
    int startingCol = currentPiece.position % GAME_CONSTANTS::BOARD_WIDTH;
    for (int row = 0; row < (GAME_CONSTANTS::PIECE_SIZE); row++)
    {
        for (int col = 0; col < (GAME_CONSTANTS::PIECE_SIZE); col++)
        {
            uint8_t pieceBitIndex = row * GAME_CONSTANTS::PIECE_SIZE + col;
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
    currentPiece.isTouchingDown = false; // touchdown time shouldn't need changed because of isTouchingDown implementation
};

uint16_t GameLogic::getRow(uint8_t rowIndex)
{
    uint16_t rowValue = 0;
    int bitIndex = rowIndex * GAME_CONSTANTS::BOARD_WIDTH;
    for (int i = 0; i < GAME_CONSTANTS::BOARD_WIDTH; i++)
    {
        if (playfield[bitIndex + i])
        {
            rowValue |= (1 << i);
        }
    }
    return rowValue;
};

bool GameLogic::isValidPosition()
{
    std::cout << "Current Position is: " << (int)currentPiece.position << std::endl; 

    // if (currentPiece.position > (GAME_CONSTANTS::BOARD_HEIGHT * GAME_CONSTANTS::BOARD_WIDTH - GAME_CONSTANTS::PIECE_SIZE))
    // { // Added for my sanity for now, may remove if redundant after fixes
    //     std::cout << "The piece's position was greater than the baord size" << std::endl;
    //     return false;
    // }
    
    uint8_t  wrapMask       = 0b0000'0000;
    uint16_t currentRowMask = 0b0000'0000'0000'0000;
    uint16_t pieceMask      = GAME_DATA::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation];
    
    uint8_t pieceRowIndex    = currentPiece.position / GAME_CONSTANTS::BOARD_WIDTH;
    uint8_t pieceColumnIndex = currentPiece.position % GAME_CONSTANTS::BOARD_WIDTH;
    
    if (pieceRowIndex > (GAME_CONSTANTS::BOARD_HEIGHT - GAME_CONSTANTS::PIECE_SIZE))
    {
        return false;
    }

    if (pieceColumnIndex > 5) // Wrapped around the board
    {
        wrapMask = GAME_DATA::WRAP_MASKS[pieceColumnIndex - 6];
    }
    
    for(int i = 0; i < GAME_CONSTANTS::PIECE_SIZE; i++)
    {
        currentRowMask =
            (
                getRow(pieceRowIndex+i)
                >> (GAME_CONSTANTS::BOARD_WIDTH - pieceColumnIndex)
            )
            & 0b1111;
        
        pieceMask >>= GAME_CONSTANTS::PIECE_SIZE; 
        currentRowMask = pieceMask & currentRowMask;
        
        if ((wrapMask & pieceMask) || currentRowMask)
        {
            return false;
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
    const std::bitset<16> pieceShape(GAME_DATA::PIECES[currentPiece.pieceIndex].rotations[currentPiece.rotation]);
    
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