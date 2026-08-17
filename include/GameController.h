#pragma once
#include "GameLogic.h"

struct GameInput
{
    bool left = false;
    bool right = false;
    bool down = false;
    bool hardDrop = false;
    bool rotateCW = false;
    bool rotateCCW = false;
    bool rotate180 = false;
    bool hold = false;
    bool reset = false;
};

class GameController
{
public:
    GameController();
    void update(float dt, const GameInput& input);
    GameLogic& getGameState() { return game; }

private:
    GameLogic game;
    GameInput prevInput;

    float dasDelay = 0.07f; // replace def 0.150
    float arrDelay = 0.0f;  // replace def 0.033
    float sdrDelay = 0.05f;

    float gravity = 0.9f;   // not sure exactly how gravity is effected

    float leftHeldTime = 0.0f;
    float rightHeldTime = 0.0f;
    float downHeldTime = 0.0f;
    float timeSinceLastDrop = 0.0f;
};