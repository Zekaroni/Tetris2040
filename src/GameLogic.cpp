#include "../include/GameLogic.h"

#include "raylib.h"
#include <cstring>

PieceGenerator::PieceGenerator() : rng(std::random_device{}()) {
    fillBag(0);
    fillBag(7);
}

void PieceGenerator::fillBag(int offset) {
    uint8_t bag[7] = {0, 1, 2, 3, 4, 5, 6};
    std::shuffle(std::begin(bag), std::end(bag), rng);
    for (int i = 0; i < 7; i++) {
        queue[offset + i] = bag[i];
    }
}

uint8_t PieceGenerator::getNextPiece() {
    uint8_t next = queue[queueIndex];
    queueIndex++;

    if (queueIndex >= 7) {
        for (int i = 0; i < 7; i++) {
            queue[i] = queue[i + 7];
        }
        fillBag(7);
        queueIndex = 0;
    }
    return next;
}

void PieceGenerator::peekQueue(uint8_t* outPieces, int count) const {
    if (count > 7) count = 7; 
    
    for (int i = 0; i < count; i++) {
        outPieces[i] = queue[queueIndex + i];
    }
}

const uint8_t* PieceGenerator::getQueue() const
{
    return queue;
}

GameLogic::GameLogic() {
    startNewGame();
}

Piece GameLogic::getCurrentPiece() const
{
     return currentPiece; 
}

uint8_t GameLogic::getCell(int x, int y) const
{ 
    return board[y][x];
}

void GameLogic::getNextQueue(uint8_t* outPieces, int count) const {
    pieceRandomizer.peekQueue(outPieces, count);
}

GameState GameLogic::getState() const
{
    return currentState;
}

int GameLogic::getLinesCleared() const
{
    return totalLinesCleared;
}

bool GameLogic::hasHeldPiece() const
{
    return !isHoldEmpty;
}

bool GameLogic::canHoldPiece() const
{
    return canHold;
}

uint8_t GameLogic::getHeldPieceIndex() const
{
    return heldPieceIndex;
}

void GameLogic::startNewGame()
{
    std::memset(board, 0, sizeof(board));
    score = 0;
    level = 1;
    isHoldEmpty = true;
    totalLinesCleared = 0;
    currentCombo = -1;

    currentState = GameState::PRACTICE;

    if (currentState == GameState::SPRINT)
    {
        sprintStartTime = std::chrono::steady_clock::now();
    }
    spawnNewPiece();
}


void GameLogic::update() {
    if (currentState == GameState::GAME_OVER) return;

    currentPiece.y += 1; 
    
    if (!isValidPosition())
    {
        currentPiece.y -= 1;
        
        if (!currentPiece.isTouchingDown)
        {
            currentPiece.isTouchingDown = true;
            currentPieceTouchdownTime = std::chrono::steady_clock::now();
        }
    } else
    {
        lastActionWasRotation = false;
    }
}

bool GameLogic::isGameOver() {
    // If a new piece spawns and immediately is invalid, game over
    return !isValidPosition();
}


void GameLogic::spawnNewPiece() {
    currentPiece.pieceIndex = pieceRandomizer.getNextPiece();
    currentPiece.x = CONSTANTS::STARTING_POSITION;
    currentPiece.y = 0;
    currentPiece.rotation = CONSTANTS::STARTING_ROTATION;
    currentPiece.isTouchingDown = false;
    canHold = true;
    lastActionWasRotation = false;
    if (!isValidPosition()) startNewGame();
}

void GameLogic::hold() {
    if (currentState == GameState::GAME_OVER) return;
    if (!canHold) return;

    if (isHoldEmpty) {
        heldPieceIndex = currentPiece.pieceIndex;
        isHoldEmpty = false;
        spawnNewPiece();
    } else {
        uint8_t temp = currentPiece.pieceIndex;
        currentPiece.pieceIndex = heldPieceIndex;
        heldPieceIndex = temp;

        // Reset the swapped piece to the top
        currentPiece.x = CONSTANTS::STARTING_POSITION;
        currentPiece.y = 0;
        currentPiece.rotation = CONSTANTS::STARTING_ROTATION;
    }
    canHold = false;
}

void GameLogic::movePiece(Direction dir) {
    if (currentState == GameState::GAME_OVER) return;

    int8_t oldX = currentPiece.x;
    
    if (dir == Direction::LEFT) currentPiece.x -= 1;
    else if (dir == Direction::RIGHT) currentPiece.x += 1;

    if (!isValidPosition()) {
        currentPiece.x = oldX; 
    } else {
        lastActionWasRotation = false;
    }

    if (currentPiece.isTouchingDown) {
        currentPieceTouchdownTime = std::chrono::steady_clock::now();
        currentPiece.y += 1;
        if (isValidPosition()) {
            currentPiece.isTouchingDown = false; 
        }
        currentPiece.y -= 1;
    }
}

void GameLogic::checkLockDelay()
{
    if (currentPiece.isTouchingDown)
    {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentPieceTouchdownTime).count();
        
        if (duration >= lockDelayAmount)
        { 
            lockPiece();
        } // FIX: Piece drops while "floating" if time is out
    }
}

void GameLogic::rotatePiece(Rotation dir) {
    if (currentState == GameState::GAME_OVER) return;
    if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::O)) return;

    uint8_t oldRot = currentPiece.rotation;
    int8_t oldX = currentPiece.x;
    int8_t oldY = currentPiece.y;
    
    // 1. Apply the mathematical rotation
    if (dir == Rotation::CLOCKWISE) {
        currentPiece.rotation = (currentPiece.rotation + 1) % 4;
    } else if (dir == Rotation::COUNTER_CLOCKWISE) {
        currentPiece.rotation = (currentPiece.rotation + 3) % 4;
    } else if (dir == Rotation::HALF_SPIN) {
        currentPiece.rotation = (currentPiece.rotation + 2) % 4;
        if (!isValidPosition()) 
        {
            currentPiece.rotation = oldRot;
        } else
        {
            lastActionWasRotation = true;
        }
        return; 
    }

    uint8_t newRot = currentPiece.rotation;
    if (isValidPosition()) 
    {
        lastActionWasRotation = true;
        return;
    }

    int kickIndex = 0;
    if      (oldRot == 0 && newRot == 1) kickIndex = 0;
    else if (oldRot == 1 && newRot == 0) kickIndex = 1;
    else if (oldRot == 1 && newRot == 2) kickIndex = 2;
    else if (oldRot == 2 && newRot == 1) kickIndex = 3;
    else if (oldRot == 2 && newRot == 3) kickIndex = 4;
    else if (oldRot == 3 && newRot == 2) kickIndex = 5;
    else if (oldRot == 3 && newRot == 0) kickIndex = 6;
    else if (oldRot == 0 && newRot == 3) kickIndex = 7;

    bool isIPiece = (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::I));

    for (int i = 0; i < 4; i++) {
        int8_t offsetX = isIPiece ? CONSTANTS::WALL_KICK_I[kickIndex][i].x : CONSTANTS::WALL_KICK_NORMAL[kickIndex][i].x;
        int8_t offsetY = isIPiece ? CONSTANTS::WALL_KICK_I[kickIndex][i].y : CONSTANTS::WALL_KICK_NORMAL[kickIndex][i].y;

        currentPiece.x = oldX + offsetX;
        currentPiece.y = oldY + offsetY;

        if (isValidPosition()) {
            lastActionWasRotation = true;
            return;
        }
    }

    // All 5 tests failed. Revert to the original state.
    currentPiece.x = oldX;
    currentPiece.y = oldY;
    currentPiece.rotation = oldRot;

    if (currentPiece.isTouchingDown) {
        currentPieceTouchdownTime = std::chrono::steady_clock::now();
        currentPiece.y += 1;
        if (isValidPosition()) {
            currentPiece.isTouchingDown = false; 
        }
        currentPiece.y -= 1;
    }
}

bool GameLogic::checkImmobility() {
    bool trapped = true;

    // Left
    currentPiece.x -= 1;
    if (isValidPosition()) trapped = false;
    currentPiece.x += 1;

    // Right
    currentPiece.x += 1;
    if (isValidPosition()) trapped = false;
    currentPiece.x -= 1;

    // Up
    currentPiece.y -= 1;
    if (isValidPosition()) trapped = false;
    currentPiece.y += 1;

    return trapped;
}

SpinType GameLogic::checkSpin() {
    if (!lastActionWasRotation) return SpinType::NONE;

    if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::T))
    {
        int filledCorners = 0;
        int corners[4][2] = {{0, 0}, {2, 0}, {0, 2}, {2, 2}};

        for (int i = 0; i < 4; i++)
        {
            int cx = currentPiece.x + corners[i][0];
            int cy = currentPiece.y + corners[i][1];
            if (
                cx < 0
                || cx >= CONSTANTS::BOARD_WIDTH
                || cy >= CONSTANTS::BOARD_HEIGHT
                || (cy >= 0 && board[cy][cx] != 0)
            )
            {
                filledCorners++;
            }
        }
        if (filledCorners >= 3) return SpinType::T_SPIN;
        return SpinType::NONE;
    }

    if (checkImmobility())
    {
        if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::Z)) return SpinType::Z_SPIN;
        if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::S)) return SpinType::S_SPIN;
        if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::L)) return SpinType::L_SPIN;
        if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::J)) return SpinType::J_SPIN;
        if (currentPiece.pieceIndex == static_cast<uint8_t>(PieceIndex::I)) return SpinType::I_SPIN;
    }

    return SpinType::NONE;
}

void GameLogic::lockPiece()
{
    lastSpin = checkSpin();
    if (lastSpin != SpinType::NONE)
    {
        // TODO: Add logic to reward spins
        TraceLog(LOG_INFO, "Special Spin Detected");
    }

    uint16_t shape = CONSTANTS::PIECES[currentPiece.pieceIndex][currentPiece.rotation];

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            if (shape & (1 << (15 - (row * 4 + col))))
            {
                board[currentPiece.y + row][currentPiece.x + col] = currentPiece.pieceIndex + 1; 
            }
        }
    }
    
    checkAndClearLines();
    spawnNewPiece();
}

void GameLogic::hardDrop()
{
    if (currentState == GameState::GAME_OVER) return;
    
    int startY = currentPiece.y;
    while (isValidPosition())
    {
        currentPiece.y += 1;
    }

    currentPiece.y -= 1; 
    if (currentPiece.y > startY)
    {
        lastActionWasRotation = false; 
    }
    
    lockPiece();
}

void GameLogic::softDrop()
{
    if (currentState == GameState::GAME_OVER) return;

    currentPiece.y += 1;
    
    if (!isValidPosition())
    {
        currentPiece.y -= 1;
        
        if (!currentPiece.isTouchingDown)
        {
            currentPiece.isTouchingDown = true;
            currentPieceTouchdownTime = std::chrono::steady_clock::now();
        }
    } else
    {
        lastActionWasRotation = false;
        currentPiece.isTouchingDown = false; 
    }
}

uint32_t GameLogic::getScore() const {
    return score;
}

uint8_t GameLogic::getLevel() const {
    return level;
}

bool GameLogic::isValidPosition() {
    uint16_t shape = CONSTANTS::PIECES[currentPiece.pieceIndex][currentPiece.rotation];

    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (shape & (1 << (15 - (row * 4 + col)))) {
                int boardX = currentPiece.x + col;
                int boardY = currentPiece.y + row;

                if (boardX < 0 || boardX >= CONSTANTS::BOARD_WIDTH || boardY >= CONSTANTS::BOARD_HEIGHT) return false;
                if (boardY >= 0 && board[boardY][boardX] != 0) return false;
            }
        }
    }
    return true;
}

int8_t GameLogic::getGhostY() {
    int8_t originalY = currentPiece.y;
    
    while (isValidPosition()) {
        currentPiece.y += 1;
    }

    int8_t ghostY = currentPiece.y - 1;
    
    currentPiece.y = originalY;
    
    return ghostY;
}

bool GameLogic::checkAndClearLines()
{
    uint8_t linesCleared = 0;

    for (int y = (CONSTANTS::BOARD_HEIGHT - 1); y >= 0; y--)
    {
        bool isFull = true;
        for (int x = 0; x < CONSTANTS::BOARD_WIDTH; x++)
        {
            if (board[y][x] == 0)
            {
                isFull = false;
                break;
            }
        }

        if (isFull)
        {
            linesCleared++;
            for (int shiftY = y; shiftY > 0; shiftY--)
            {
                for (int x = 0; x < CONSTANTS::BOARD_WIDTH; x++)
                {
                    board[shiftY][x] = board[shiftY - 1][x];
                }
            }
            for (int x = 0; x < CONSTANTS::BOARD_WIDTH; x++)
            {
                board[0][x] = 0;
            }
            y++; 
        }
    }

    // TODO: This is for detecting a PC for scoring
    // bool isPerfectClear = true;
    // for (int x = 0; x < CONSTANTS::BOARD_WIDTH; x++)
    // {
    //     if (board[CONSTANTS::BOARD_HEIGHT - 1][x] != 0)
    //     {
    //         isPerfectClear = false;
    //         break;
    //     }
    // }

    if (linesCleared > 0)
    {
        totalLinesCleared += linesCleared;
        currentCombo++;
        
        // Sprint Check
        if ((totalLinesCleared >= sprintLineCount) && (currentState == GameState::SPRINT))
        {
            sprintEndTime = std::chrono::steady_clock::now();
            currentState = GameState::GAME_OVER;

            auto rawDuration = sprintEndTime - sprintStartTime;
            float finalTime = std::chrono::duration<float>(rawDuration).count();    
            TraceLog(LOG_INFO, "Sprint finished in %.3f seconds!", finalTime);
        }
        
        updateScore(linesCleared);
        return true;
    }

    currentCombo = -1; 
    return false;
}

void GameLogic::updateScore(uint8_t linesCleared)
{
    // TODO: Add special scoring logic for thigns like Ultra mode and marathon
    score += (linesCleared * 100) * level; 
}