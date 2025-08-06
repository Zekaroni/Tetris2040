#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <cstdint>
#include <bitset>
#include <deque>
#include <random>
#include <algorithm>
#include <chrono>

using CurrentTime = std::chrono::steady_clock::time_point; // Alias for better readability

namespace GAME_CONSTANTS
{
    constexpr uint8_t PIECE_COUNT       = 7;
    constexpr uint8_t ROTATION_COUNT    = 4;
    constexpr uint8_t PIECE_SIZE        = 4; // x * x bounding box
    constexpr uint8_t BOARD_WIDTH       = 10;
    constexpr uint8_t BOARD_HEIGHT      = 20;
    constexpr uint8_t STARTING_POSITION = 4;
    constexpr uint8_t STARTING_ROTATION = 0;
    constexpr uint8_t BAG_MIN_LIMIT     = 14;
    constexpr uint8_t STARTING_GRAVITY  = 1;
    constexpr uint8_t TILE_COUNT        = BOARD_HEIGHT * BOARD_WIDTH;
    constexpr uint8_t FULL_LINE_MASK    = (1 << BOARD_WIDTH) - 1; // Favorite line so far
    const std::bitset<TILE_COUNT> FULL_BOARD_MASK = std::bitset<TILE_COUNT>().set();
};

namespace CLEAR_SCORINGS
{ // NOTE: I do not care about sd or hd points... they are like so little
    // Standard Clears
    constexpr uint16_t SINGLE_LINE = 100;
    constexpr uint16_t DOUBLE_LINE = 300;
    constexpr uint16_t TRIPLE_LINE = 500;
    constexpr uint16_t TETRIS      = 800;

    // T-Spins Clears
    constexpr uint16_t TSPIN_SINGLE = 800;
    constexpr uint16_t TSPIN_DOUBLE = 1200;
    constexpr uint16_t TSPIN_TRIPLE = 1600;

    // B2B Clears
    constexpr uint16_t B2B_TETRIS       = 1200;
    constexpr uint16_t B2B_TSPIN_SINGLE = 1200;
    constexpr uint16_t B2B_TSPIN_DOUBLE = 1800;
    constexpr uint16_t B2B_TSPIN_TRIPLE = 2400;

    // NOTE: Do I want to add all spin scoring? I think that could be fun.
};

enum Direction{LEFT, RIGHT};
enum Rotation{CLOCKWISE, COUNTER_CLOCKWISE, HALF_SPIN /*180*/};
enum PieceIndex{I,O,T,S,Z,J,L,NULL_PIECE}; // This is piece -> index order

struct PieceProperties
{ // color was removed and instead will be picked up by graphic engine with pience index
    const uint16_t rotations[GAME_CONSTANTS::ROTATION_COUNT];
};

struct TileAttributes
{ // 1 bytes total
    uint8_t pieceIndex: 3; // 0-6 relates to a piece (if 7, no piece)
    uint8_t state:      4; // TBD what these four bits can be for
    uint8_t flashing:   1; // Something for line clears maybe?
};

struct Piece
{ // 2 bytes total
    uint8_t position;          // Where the TOP-LEFT of the piece bitmask is
    uint8_t pieceIndex:     3; // Three bits for the piece index 0-7
    uint8_t rotation:       2; // Two bits for rotation: 0, 90, 180, 270
    uint8_t isTouchingDown: 1; // true if piece touched ground
};

namespace GameData
{
    const PieceProperties PIECES[GAME_CONSTANTS::PIECE_COUNT] = 
    {
      { // I Piece
            {
                0b0000111100000000,
                0b0010001000100010,
                0b0000000011110000,
                0b0100010001000100
            },
        },
        { // O Piece
            {
                0b0110011000000000,
                0b0110011000000000,
                0b0110011000000000,
                0b0110011000000000
            },
        },
        { // T Piece
            {
                0b0100111000000000,
                0b0100011001000000,
                0b0000111001000000,
                0b0100110001000000
            },
        },
        { // S Piece
            {
                0b0110011000000000,
                0b0100011000100000,
                0b0000011011000000,
                0b1000110001000000
            },
        },
        { // Z Piece
            {
                0b1100011000000000,
                0b0010011001000000,
                0b0000110001100000,
                0b0100110010000000
            },
        },
        { // J Piece
            {
                0b1000111000000000,
                0b0110010001000000,
                0b0000111000100000,
                0b0100010011000000
            },
        },
        { // L Piece
            {
                0b0010111000000000,
                0b0100010001100000,
                0b0000111010000000,
                0b1100010001000000
            },
        }
    };

    // TODO: I will have to make masks for every possible T-Spin and other speical case
    // for each piece
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
    std::bitset<GAME_CONSTANTS::TILE_COUNT> playfield;
    TileAttributes tileData[GAME_CONSTANTS::TILE_COUNT];
    CurrentTime currentPieceTouchdownTime;
    
    void generateNewPiece();
    uint16_t getRow(uint8_t rowIndex);
    void updateRows(uint8_t endRow, uint8_t rowCount);
    bool isValidPosition();
    bool checkAndClearLines();
    void updateScore(uint8_t linesCleared);
};

#endif