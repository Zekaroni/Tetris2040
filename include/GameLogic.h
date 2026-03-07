#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <cstdint>
#include <bitset>
#include <deque>
#include <random>
#include <algorithm>
#include <chrono>

#include <fstream> // DEBUG ONLY
#include <string>

using CurrentTime = std::chrono::steady_clock::time_point; // Alias for better readability

enum Direction{LEFT, RIGHT, DOWN};
enum Rotation{CLOCKWISE = 1, COUNTER_CLOCKWISE = -1, HALF_SPIN = 2 /*180*/};
enum PieceIndex{I,O,T,S,Z,J,L,NULL_PIECE}; // This is piece -> index order


namespace GAME_CONSTANTS
{
    constexpr uint8_t PIECE_COUNT       = 7;  // I, O, T, S, Z, J, L
    constexpr uint8_t ROTATION_COUNT    = 4;  // 0, 90, 180, 270
    constexpr uint8_t PIECE_SIZE        = 4;  // x * x bounding box
    constexpr uint8_t BOARD_WIDTH       = 10; // These would be fun to change later on, but current logic would break
    constexpr uint8_t BOARD_HEIGHT      = 20; // ^^^
    constexpr uint8_t STARTING_POSITION = 14; // Due to current wrapping issues, would like to FIX
    constexpr uint8_t STARTING_ROTATION = 0;  // Starts with default rotation
    constexpr uint8_t BAG_MIN_LIMIT     = 14; // Minimum amount of pieces in the bag queue
    constexpr uint8_t STARTING_GRAVITY  = 1;  // Speed that the pieces fall, may change later
    constexpr uint8_t TILE_COUNT        = BOARD_HEIGHT * BOARD_WIDTH;
    constexpr uint16_t FULL_LINE_MASK    = (1 << BOARD_WIDTH) - 1; // Favorite line so far
    const std::bitset<TILE_COUNT> FULL_BOARD_MASK = std::bitset<TILE_COUNT>().set();
    constexpr uint16_t SCORE_LOOKUP_TABLE[16] = {
        0, 100, 200, 500,  800,  800, 1200, 1600,
        0, 100, 200, 500, 1200, 1200, 1800, 2400
    };
};

struct PieceProperties
{ // color was removed and instead will be picked up by graphic engine with pience index
    const uint16_t rotations[GAME_CONSTANTS::ROTATION_COUNT];
};

namespace GAME_DATA
{
    const PieceProperties PIECES[GAME_CONSTANTS::PIECE_COUNT] = 
    {
      { // I Piece
            { // LSN Completed
                0b0000'0000'1111'0000,
                0b0010'0010'0010'0010,
                0b0000'1111'0000'0000,
                0b0100'0100'0100'0100
            },
        },
        { // O Piece
            {
                0b0110'0110'0000'0000,
                0b0110'0110'0000'0000,
                0b0110'0110'0000'0000,
                0b0110'0110'0000'0000
            },
        },
        { // T Piece
            {
                0b0100'1110'0000'0000,
                0b0100'0110'0100'0000,
                0b0000'1110'0100'0000,
                0b0100'1100'0100'0000
            },
        },
        { // S Piece
            {
                0b0110'1100'0000'0000,
                0b0100'0110'0010'0000,
                0b0000'0110'1100'0000,
                0b1000'1100'0100'0000
            },
        },
        { // Z Piece
            {
                0b1100'0110'0000'0000,
                0b0010'0110'0100'0000,
                0b0000'1100'0110'0000,
                0b0100'1100'1000'0000
            },
        },
        { // J Piece
            {
                0b1000'1110'0000'0000,
                0b0110'0100'0100'0000,
                0b0000'1110'0010'0000,
                0b0100'0100'1100'0000
            },
        },
        { // L Piece
            {
                0b0010'1110'0000'0000,
                0b0100'0100'0110'0000,
                0b0000'1110'1000'0000,
                0b1100'0100'0100'0000
            },
        }
    };
    
    constexpr uint8_t WRAP_MASKS[4] = { // ORDER MATTERS HERE
        0b0000'0001, // Wrapped once to the right
        0b0000'0011, // Wrapped twice to the right
        0b0000'1100, // Wrapped twice to the left
        0b0000'1000  // Wrapped once to the left
    };
};

union GameState
{ // 1 byte
    struct 
    {
        uint8_t unused:     4;
        bool    isTSpin:    1;
        bool    isCombo:    1;  
        bool    isBTB:      1;
        bool    isWrapping: 1;
    } bits;
    uint8_t fullByte;
};

struct TileAttributes
{ // 1 bytes
    uint8_t pieceIndex: 3; // 0-6 relates to a piece (if 7, no piece)
    uint8_t state:      4; // TBD what these four bits can be for
    uint8_t flashing:   1; // Something for line clears maybe?
};
    
struct Piece
{ // 2 bytes
    uint8_t position;          // Where the TOP-LEFT of the piece bitmask is
    uint8_t pieceIndex:     3; // Three bits for the piece index 0-7
    uint8_t rotation:       2; // Two bits for rotation: 0, 90, 180, 270
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
    
    void DAS(Direction dir);
    bool rotatePiece(Rotation dir);
    bool softDrop();
    bool hardDrop();
    
    uint64_t getScore() const;
    uint8_t getLevel() const;
    
    void printBoard() const;
    
    private:
    PieceGenerator pieceRandomizer;
    Piece currentPiece;
    uint64_t score;
    uint8_t level;  
    std::bitset<GAME_CONSTANTS::TILE_COUNT> playfield;
    TileAttributes tileData[GAME_CONSTANTS::TILE_COUNT];
    CurrentTime currentPieceTouchdownTime;
    GameState gamestate;
    
    void generateNewPiece();
    bool movePiece(Direction dir);
    bool revertPiece(Direction dir);
    void placePiece();
    uint16_t getRow(uint8_t rowIndex);
    void updateRows(uint8_t endRow, uint8_t rowCount);
    bool isValidPosition();
    bool checkAndClearLines();
    void updateScore(uint8_t linesCleared);
    std::bitset<GAME_CONSTANTS::TILE_COUNT> getPlayfield() const;


    // For DEBUG
    std::ofstream logFile;
    void logBoardState();
};

#endif