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

struct PieceProperties
{
    const std::array<std::bitset<16>, CONSTANTS::ROTATION_COUNT> rotations;
    uint16_t color; // TODO: Add an 8 bit pallete instead of 16bit color
                    // This allows each engine to handle the color differently
};

namespace GameData
{
    inline constexpr std::array<PieceProperties, CONSTANTS::PIECE_COUNT> PIECES = 
    {{
        // I Piece
        {
            {{
                std::bitset<16>{0b0000111100000000},
                std::bitset<16>{0b0010001000100010},
                std::bitset<16>{0b0000000011110000},
                std::bitset<16>{0b0100010001000100}
            }},
            0x0000 // TODO: Add all the colors
        },
        // O Piece
        {
            {{
                std::bitset<16>{0b0110011000000000},
                std::bitset<16>{0b0110011000000000},
                std::bitset<16>{0b0110011000000000},
                std::bitset<16>{0b0110011000000000}
            }},
            0x0000
        },
        // T Piece
        {
            {{
                std::bitset<16>{0b0100111000000000},
                std::bitset<16>{0b0100011001000000},
                std::bitset<16>{0b0000111001000000},
                std::bitset<16>{0b0100110001000000}
            }},
            0x0000
        },
        // S Piece
        {
            {{
                std::bitset<16>{0b0110011000000000},
                std::bitset<16>{0b0100011000100000},
                std::bitset<16>{0b0000011011000000},
                std::bitset<16>{0b1000110001000000}
            }},
            0x0000
        },
        // Z Piece
        {
            {{
                std::bitset<16>{0b1100011000000000},
                std::bitset<16>{0b0010011001000000},
                std::bitset<16>{0b0000110001100000},
                std::bitset<16>{0b0100110010000000}
            }},
            0x0000
        },
        // J Piece
        {
            {{
                std::bitset<16>{0b1000111000000000},
                std::bitset<16>{0b0110010001000000},
                std::bitset<16>{0b0000111000100000},
                std::bitset<16>{0b0100010011000000}
            }},
            0x0000
        },
        // L Piece
        {
            {{
                std::bitset<16>{0b0010111000000000},
                std::bitset<16>{0b0100010001100000},
                std::bitset<16>{0b0000111010000000},
                std::bitset<16>{0b1100010001000000}
            }},
            0x0000
        }
    }};
};

enum class PieceIndex{I,O,T,S,Z,J,L};
enum class Direction{LEFT, RIGHT};
enum class Rotation{CLOCKWISE, COUNTER_CLOCKWISE, HALF_SPIN /*180*/};

struct TileAttributes
{
    uint8_t pieceIndex: 3; // 0-7 relates to a piece
    uint8_t state : 4;     // TBD what these four bits can be for
    uint8_t flashing: 1;   // Something for line clears maybe?
};

struct Piece
{ // 11 bytes total
    uint16_t position;         // Where the TOP-LEFT of the piece bitmask is
    uint8_t pieceIndex: 3;     // Three bits for the piece index 0-7
    uint8_t rotation: 2;       // Two bits for rotation: 0, 90, 180, 270
    uint8_t isTouchingDown: 1; // A bool check that take use of "free" memory
    CurrentTime touchdownTime; // 8 bytes
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
    std::array<TileAttributes, CONSTANTS::TILE_COUNT> tileData;
    
    void spawnNewPiece();
    std::bitset<CONSTANTS::BOARD_WIDTH> getRow(uint8_t rowIndex);
    void updateRows(uint8_t endRow, uint8_t rowCount);
    bool isValidPosition();
    bool checkAndClearLines();
    void updateScore(uint8_t linesCleared);
};

#endif