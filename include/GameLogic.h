#pragma once
#include <cstdint>
#include <deque>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

#include "Constants.h"

using TimePoint = std::chrono::steady_clock::time_point; // Alias for better readability

enum class PieceIndex{I,O,T,S,Z,J,L}; // This is piece -> index order
enum class Direction{LEFT, RIGHT};
enum class Rotation{CLOCKWISE, COUNTER_CLOCKWISE, HALF_SPIN /*180*/};
enum class GameState {PRACTICE, SPRINT, ULTRA, GAME_OVER};
enum class SpinType {NONE, T_SPIN, Z_SPIN, S_SPIN, L_SPIN, J_SPIN, I_SPIN};

struct Piece
{ 
    int8_t x;           // Signed to allow -1 or -2 for empty columns
    int8_t y;                  
    uint8_t pieceIndex;     
    uint8_t rotation;       
    bool isTouchingDown; 
};;

class PieceGenerator {
public:
    PieceGenerator();
    uint8_t getNextPiece();
    const uint8_t* getQueue() const;
    void peekQueue(uint8_t* outPieces, int count) const;

private:
    void fillBag(int offset);
    uint8_t queue[14];
    uint8_t queueIndex = 0; 
    std::mt19937 rng;
};


class GameLogic
{
public:
    GameLogic();
    void update();       // Move the game forward one "tick"
    void startNewGame(); 
    
    void movePiece(Direction dir);
    void rotatePiece(Rotation dir);
    void softDrop();
    void hardDrop();
    void hold();
    
    Piece getCurrentPiece() const;
    uint8_t getHeldPieceIndex() const;
    void getNextQueue(uint8_t* outPieces, int count) const;
    
    uint32_t getScore() const;
    uint8_t getLevel() const;
    int getCombo() const;
    uint8_t getCell(int x, int y) const;
    GameState getState() const;
    int getLinesCleared() const;
    int8_t getGhostY();

    void checkLockDelay();
    bool isGameOver();
    bool hasHeldPiece() const;
    bool canHoldPiece() const;
    
private:
    uint8_t board[CONSTANTS::BOARD_HEIGHT][CONSTANTS::BOARD_WIDTH];
    PieceGenerator pieceRandomizer;
    GameState currentState;
    TimePoint currentPieceTouchdownTime;
    TimePoint sprintStartTime;
    TimePoint sprintEndTime;
    Piece currentPiece;
    SpinType lastSpin = SpinType::NONE;
    uint32_t score             = 0;
    uint8_t level              = 0;
    uint32_t totalLinesCleared = 0; 
    uint8_t heldPieceIndex     = 0;
    uint16_t lockDelayAmount   = 500; // in ms
    int sprintLineCount        = 10;
    int currentCombo           = 0;
    bool isHoldEmpty           = true;
    bool canHold               = true;
    bool lastActionWasRotation = false;
    
    SpinType checkSpin();
    bool checkImmobility();
    void lockPiece();
    void spawnNewPiece();
    bool isValidPosition();
    bool checkAndClearLines();
    void updateScore(uint8_t linesCleared);
};