#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>
#include <bitset>

namespace CONSTANTS
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
    constexpr uint8_t FULL_LINE_MASK    = (1 << BOARD_WIDTH) - 1; // Favorite line so far
    constexpr uint8_t TILE_COUNT        = BOARD_HEIGHT * BOARD_WIDTH;
    const std::bitset<TILE_COUNT> FULL_BOARD_MASK = std::bitset<TILE_COUNT>().set();
};

#endif