#pragma once
#include <cstdint>

namespace CONSTANTS {
    constexpr uint8_t BAG_MIN_LIMIT     = 7;
    constexpr uint8_t STARTING_POSITION = 3; // Keep 3 so SRS bounding boxes center perfectly
    constexpr uint8_t STARTING_ROTATION = 0;
    constexpr int8_t BOARD_HEIGHT      = 22;
    constexpr int8_t BOARD_WIDTH       = 10;
    constexpr int8_t VISIBILE_HEIGHT   = 20;

    // Super Rotation System (SRS) standard bitboards
    // Formatted in binary: each block of 4 bits is a row top-to-bottom.
    // Order: I, O, T, S, Z, J, L
    const uint16_t PIECES[7][4] =
    {
        // I Piece (4x4 bounding box)
        {0b0000'1111'0000'0000, 0b0010'0010'0010'0010, 0b0000'0000'1111'0000, 0b0100'0100'0100'0100},
        
        // O Piece (Centered in 4x4, doesn't rotate)
        {0b0110'0110'0000'0000, 0b0110'0110'0000'0000, 0b0110'0110'0000'0000, 0b0110'0110'0000'0000},
        
        // T Piece (Top-left 3x3 bounding box)
        {0b0100'1110'0000'0000, 0b0100'0110'0100'0000, 0b0000'1110'0100'0000, 0b0100'1100'0100'0000},
        
        // S Piece (Top-left 3x3 bounding box)
        {0b0110'1100'0000'0000, 0b0100'0110'0010'0000, 0b0000'0110'1100'0000, 0b1000'1100'0100'0000},
        
        // Z Piece (Top-left 3x3 bounding box)
        {0b1100'0110'0000'0000, 0b0010'0110'0100'0000, 0b0000'1100'0110'0000, 0b0100'1100'1000'0000},
        
        // J Piece (Top-left 3x3 bounding box)
        {0b1000'1110'0000'0000, 0b0110'0100'0100'0000, 0b0000'1110'0010'0000, 0b0100'0100'1100'0000},
        
        // L Piece (Top-left 3x3 bounding box)
        {0b0010'1110'0000'0000, 0b0100'0100'0110'0000, 0b0000'1110'1000'0000, 0b1100'0100'0100'0000}
    };

    struct Point { int8_t x; int8_t y; };

    // Standard SRS Kick Table (Y-axis inverted for 2D array coordinates)
    // 8 state changes, 4 tests each (Test 1 is always 0,0 and is handled in the logic)
    // Indices mapping:
    // 0: 0->1 | 1: 1->0 | 2: 1->2 | 3: 2->1 | 4: 2->3 | 5: 3->2 | 6: 3->0 | 7: 0->3
    const Point WALL_KICK_NORMAL[8][4] =
    {
        {{-1, 0}, {-1, -1}, {0,  2}, {-1,  2}}, // 0: 0 -> 1
        {{ 1, 0}, { 1,  1}, {0, -2}, { 1, -2}}, // 1: 1 -> 0
        {{ 1, 0}, { 1,  1}, {0, -2}, { 1, -2}}, // 2: 1 -> 2
        {{-1, 0}, {-1, -1}, {0,  2}, {-1,  2}}, // 3: 2 -> 1
        {{ 1, 0}, { 1, -1}, {0,  2}, { 1,  2}}, // 4: 2 -> 3
        {{-1, 0}, {-1,  1}, {0, -2}, {-1, -2}}, // 5: 3 -> 2
        {{-1, 0}, {-1,  1}, {0, -2}, {-1, -2}}, // 6: 3 -> 0
        {{ 1, 0}, { 1, -1}, {0,  2}, { 1,  2}}  // 7: 0 -> 3
    };

    const Point WALL_KICK_I[8][4] =
    {
        {{-2, 0}, { 1, 0}, {-2,  1}, { 1, -2}}, // 0: 0 -> 1
        {{ 2, 0}, {-1, 0}, { 2, -1}, {-1,  2}}, // 1: 1 -> 0
        {{-1, 0}, { 2, 0}, {-1, -2}, { 2,  1}}, // 2: 1 -> 2
        {{ 1, 0}, {-2, 0}, { 1,  2}, {-2, -1}}, // 3: 2 -> 1
        {{ 2, 0}, {-1, 0}, { 2, -1}, {-1,  2}}, // 4: 2 -> 3
        {{-2, 0}, { 1, 0}, {-2,  1}, { 1, -2}}, // 5: 3 -> 2
        {{ 1, 0}, {-2, 0}, { 1,  2}, {-2, -1}}, // 6: 3 -> 0
        {{-1, 0}, { 2, 0}, {-1, -2}, { 2,  1}}  // 7: 0 -> 3
    };
}