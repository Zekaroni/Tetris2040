#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <array>
#include <cstdint>
#include <bitset>
#include <deque>
#include <random>
#include <algorithm>
#include <chrono>

#include "./include/Constants.h"

using CurrentTime = std::chrono::steady_clock::time_point; // Alias for better readability

enum class PIECE_INDEX {I,O,T,S,Z,J,L};
namespace PIECES
{
    const std::array<const std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>, CONSTANTS::PIECE_COUNT> ROATATIONS = 
    {
        // I Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b0000111100000000},
            std::bitset<16>{0b0010001000100010},
            std::bitset<16>{0b0000000011110000},
            std::bitset<16>{0b0100010001000100}
        },
        // O Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b0110011000000000},
            std::bitset<16>{0b0110011000000000},
            std::bitset<16>{0b0110011000000000},
            std::bitset<16>{0b0110011000000000}
        },
        // T Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b0100111000000000},
            std::bitset<16>{0b0100011001000000},
            std::bitset<16>{0b0000111001000000},
            std::bitset<16>{0b0100110001000000}
        },
        // S Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b0110011000000000},
            std::bitset<16>{0b0100011000100000},
            std::bitset<16>{0b0000011011000000},
            std::bitset<16>{0b1000110001000000}
        },
        // Z Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b1100011000000000},
            std::bitset<16>{0b0010011001000000},
            std::bitset<16>{0b0000110001100000},
            std::bitset<16>{0b0100110010000000}
        },
        // J Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b1000111000000000},
            std::bitset<16>{0b0110010001000000},
            std::bitset<16>{0b0000111000100000},
            std::bitset<16>{0b0100010011000000}
        },
        // L Piece
        std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT>
        {
            std::bitset<16>{0b0010111000000000},
            std::bitset<16>{0b0100010001100000},
            std::bitset<16>{0b0000111010000000},
            std::bitset<16>{0b1100010001000000}
        }
    };

    const std::array<uint16_t, CONSTANTS::PIECE_COUNT> COLORS = 
    { // TODO: Add colors
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000,
        0b0000000000000000
    };
};


struct TileAttributes
{
    uint8_t pieceIndex: 3; // 0-7 relates to a piece
    uint8_t state : 4;     // TBD what these four bits can be for
    uint8_t flashing: 1;   // Something for line clears maybe?
};

struct Piece
{ // 11 bytes total
    uint16_t position; // Where the TOP-LEFT of the piece bitmask is
    uint8_t pieceIndex: 3; // Three bits for the piece index 0-7
    uint8_t rotation: 2; // Two bits for rotation: 0, 90, 180, 270
    CurrentTime touchdownTime; // 8 bytes
};

struct Direction
{

};

struct Rotation
{

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
    Piece currentPiece;
    uint32_t score;
    uint8_t level;
    std::bitset<CONSTANTS::TILE_COUNT> playfield;
    std::array<TileAttributes, CONSTANTS::TILE_COUNT> tileData;
    bool isValidPosition();
    void spawnNewPiece();
    uint8_t checkAndClearLines();
    void clearLine(uint8_t row);
    void updateScore(uint8_t linesCleared);
};

#endif