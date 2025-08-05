#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <cstdint>
#include <bitset>
#include <deque>
#include <random>
#include <algorithm>
#include <chrono>

#include "./include/Constants.h"

using CurrentTime = std::chrono::steady_clock::time_point; // Alias for better readability

struct PieceProperties
{ // color was removed and instead will be picked up by graphic engine with pience index
    const uint16_t rotations[CONSTANTS::ROTATION_COUNT];
};

namespace GameData
{
    const PieceProperties PIECES[CONSTANTS::PIECE_COUNT] = 
    {
        // I Piece
        {
            {
                0b0000111100000000,
                0b0010001000100010,
                0b0000000011110000,
                0b0100010001000100
            },
        },
        // O Piece
        {
            {
                0b0110011000000000,
                0b0110011000000000,
                0b0110011000000000,
                0b0110011000000000
            },
        },
        // T Piece
        {
            {
                0b0100111000000000,
                0b0100011001000000,
                0b0000111001000000,
                0b0100110001000000
            },
        },
        // S Piece
        {
            {
                0b0110011000000000,
                0b0100011000100000,
                0b0000011011000000,
                0b1000110001000000
            },
        },
        // Z Piece
        {
            {
                0b1100011000000000,
                0b0010011001000000,
                0b0000110001100000,
                0b0100110010000000
            },
        },
        // J Piece
        {
            {
                0b1000111000000000,
                0b0110010001000000,
                0b0000111000100000,
                0b0100010011000000
            },
        },
        // L Piece
        {
            {
                0b0010111000000000,
                0b0100010001100000,
                0b0000111010000000,
                0b1100010001000000
            },
        }
    };
};

enum class PieceIndex{I,O,T,S,Z,J,L}; // This is piece -> index order
enum class Direction{LEFT, RIGHT};
enum class Rotation{CLOCKWISE, COUNTER_CLOCKWISE, HALF_SPIN /*180*/};

struct TileAttributes
{ // 1 bytes total
    uint8_t pieceIndex: 3; // 0-6 relates to a piece (if 7, no piece)
    uint8_t state : 4;     // TBD what these four bits can be for
    uint8_t flashing: 1;   // Something for line clears maybe?
};

struct Piece
{ // 2 bytes total
    uint8_t position;          // Where the TOP-LEFT of the piece bitmask is
    uint8_t pieceIndex: 3;     // Three bits for the piece index 0-7
    uint8_t rotation: 2;       // Two bits for rotation: 0, 90, 180, 270
    uint8_t isTouchingDown: 1; // true if piece touched ground
};

class PieceGenerator {
    public:
    PieceGenerator();
    uint8_t getNextPiece();
    const std::deque<uint8_t>& getQueue() const;
    
    private:
    void fillQueue();
    std::vector<uint8_t> bag;
    std::deque<uint8_t> queue;
    std::mt19937 rng;
};

class GameLogic
{
    public:
    GameLogic();
    void startNewGame(); 
    void update();       // Move the game forward one "tick"
    bool isGameOver();   // Checking for game loss
    
    void movePiece(Direction dir);
    void rotatePiece(Rotation dir);
    void softDrop();
    void hardDrop();
    
    uint32_t getScore() const;
    uint8_t getLevel() const;
    
    private:
    PieceGenerator pieceRandomizer;
    Piece currentPiece;
    uint32_t score;
    uint8_t level;  
    std::bitset<CONSTANTS::TILE_COUNT> playfield;
    TileAttributes tileData[CONSTANTS::TILE_COUNT];
    CurrentTime currentPieceTouchdownTime;
    
    void generateNewPiece();
    std::bitset<CONSTANTS::BOARD_WIDTH> getRow(uint8_t rowIndex);
    void updateRows(uint8_t endRow, uint8_t rowCount);
    bool isValidPosition();
    bool checkAndClearLines();
    void updateScore(uint8_t linesCleared);
};

#endif