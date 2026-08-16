#include "../include/GameController.h"

GameController::GameController() {}

void GameController::update(float dt, const GameInput& input) {
    if (input.hardDrop && !prevInput.hardDrop) game.hardDrop();
    if (input.rotateCW && !prevInput.rotateCW) game.rotatePiece(Rotation::CLOCKWISE);
    if (input.rotateCCW && !prevInput.rotateCCW) game.rotatePiece(Rotation::COUNTER_CLOCKWISE);
    if (input.rotate180 && !prevInput.rotate180) game.rotatePiece(Rotation::HALF_SPIN);
    if (input.hold && !prevInput.hold) game.hold();
    if (input.reset && !prevInput.reset) game.startNewGame();

    if (input.left && !prevInput.left) {
        game.movePiece(Direction::LEFT);
        leftHeldTime = 0.0f;
    } else if (input.left) {
        leftHeldTime += dt;
        if (leftHeldTime >= dasDelay) {
            if (arrDelay <= 0.001f) {
                int8_t prevX;
                do {
                    prevX = game.getCurrentPiece().x;
                    game.movePiece(Direction::LEFT);
                } while (game.getCurrentPiece().x != prevX);
                leftHeldTime = dasDelay; 
            } else {
                while (leftHeldTime >= dasDelay) {
                    game.movePiece(Direction::LEFT);
                    leftHeldTime -= arrDelay;
                }
            }
        }
    } else {
        leftHeldTime = 0.0f;
    }

    if (input.right && !prevInput.right) {
        game.movePiece(Direction::RIGHT);
        rightHeldTime = 0.0f;
    } else if (input.right) {
        rightHeldTime += dt;
        if (rightHeldTime >= dasDelay) {
            if (arrDelay <= 0.001f) {
                int8_t prevX;
                do {
                    prevX = game.getCurrentPiece().x;
                    game.movePiece(Direction::RIGHT);
                } while (game.getCurrentPiece().x != prevX);
                rightHeldTime = dasDelay; 
            } else {
                while (rightHeldTime >= dasDelay) {
                    game.movePiece(Direction::RIGHT);
                    rightHeldTime -= arrDelay;
                }
            }
        }
    } else {
        rightHeldTime = 0.0f;
    }

    if (input.down && !prevInput.down) {
        game.softDrop();
        downHeldTime = 0.0f;
        timeSinceLastDrop = 0.0f; 
    } else if (input.down) {
        downHeldTime += dt;
        if (sdrDelay <= 0.001f) {
            while (!game.getCurrentPiece().isTouchingDown) game.softDrop();
            downHeldTime = 0.0f;
        } else {
            while (downHeldTime >= sdrDelay) {
                if (game.getCurrentPiece().isTouchingDown) break;
                game.softDrop();
                downHeldTime -= sdrDelay;
            }
        }
        timeSinceLastDrop = 0.0f; 
    } else {
        downHeldTime = 0.0f;
    }

    timeSinceLastDrop += dt;
    if (timeSinceLastDrop >= gravity) {
        game.update();
        timeSinceLastDrop = 0.0f;
    }

    game.checkLockDelay();
    prevInput = input;
}