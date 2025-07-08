#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

namespace CONSTANTS
{
    constexpr uint8_t BOARD_WIDTH  = 10;
    constexpr uint8_t BOARD_HEIGHT = 20;
    // constexpr uint8_t BOARD_MAX_HEIGHT = 20;
    constexpr uint16_t TILE_COUNT = BOARD_HEIGHT * BOARD_WIDTH;
    constexpr uint8_t PIECE_COUNT = 7;
    constexpr uint8_t ROTATION_COUNT = 4;

    constexpr uint8_t BAG_MIN_LIMIT = 7;
    constexpr uint16_t STARTING_POSITION = 4;
    constexpr uint8_t STARTING_ROTATION = 0;
};

#endif