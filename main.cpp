#include "raylib.h"
#include "./include/GameController.h"


int main() 
{
    const int screenWidth = 320;
    const int screenHeight = 480;
    const int blockSize = 24; 

    Color colors[8] = { BLACK, SKYBLUE, YELLOW, PURPLE, GREEN, RED, BLUE, ORANGE };
    InitWindow(screenWidth, screenHeight, "Tetris Project");
    SetTargetFPS(30); 

    GameController engine;
    while (!WindowShouldClose()) 
    {
        GameInput currentInput;
        currentInput.left = IsKeyDown(KEY_A);
        currentInput.right = IsKeyDown(KEY_D);
        currentInput.down = IsKeyDown(KEY_S);
        currentInput.hardDrop = IsKeyDown(KEY_W); 
        currentInput.rotateCW = IsKeyDown(KEY_J);
        currentInput.rotateCCW = IsKeyDown(KEY_K);
        currentInput.rotate180 = IsKeyDown(KEY_L);
        currentInput.hold = IsKeyDown(KEY_SPACE);
        currentInput.reset = IsKeyDown(KEY_BACKSPACE);
        
        engine.update(GetFrameTime(), currentInput);
        GameLogic& game = engine.getGameState();

        BeginDrawing();
        ClearBackground(DARKGRAY);

        int boardOffsetX = 0; 
        DrawRectangle(boardOffsetX, 0, 10 * blockSize, 20 * blockSize, BLACK);
        for (int y = 2; y < 22; y++) 
        {
            for (int x = 0; x < 10; x++) 
            {
                uint8_t cell = game.getCell(x, y);
                if (cell > 0) 
                {
                    DrawRectangle(boardOffsetX + (x * blockSize), (y - 2) * blockSize, blockSize - 1, blockSize - 1, colors[cell]);
                }
            }
        }

        Piece current = game.getCurrentPiece();
        uint16_t shape = CONSTANTS::PIECES[current.pieceIndex][current.rotation];
        int8_t ghostY = game.getGhostY();
        for (int row = 0; row < 4; ++row) 
        {
            for (int col = 0; col < 4; ++col) 
            {
                if ((shape & (1 << (15 - (row * 4 + col)))) && (ghostY + row >= 2)) 
                {
                    int drawX = boardOffsetX + ((current.x + col) * blockSize);
                    int drawY = ((ghostY + row) - 2) * blockSize;
                    DrawRectangleLines(drawX, drawY, blockSize, blockSize, colors[current.pieceIndex + 1]); 
                }
            }
        }

        // Draw Active Piece
        for (int row = 0; row < 4; ++row) 
        {
            for (int col = 0; col < 4; ++col) 
            {
                if ((shape & (1 << (15 - (row * 4 + col)))) && (current.y + row >= 2)) 
                {
                    int drawX = boardOffsetX + ((current.x + col) * blockSize);
                    int drawY = ((current.y + row) - 2) * blockSize;
                    DrawRectangle(drawX, drawY, blockSize - 1, blockSize - 1, colors[current.pieceIndex + 1]); 
                }
            }
        }

        int uiBlockSize = 14; 
        int uiOffsetX = boardOffsetX + (10 * blockSize) + 16; 
        int uiOffsetY = 32;

        DrawText("NEXT", uiOffsetX, 10, 20, RAYWHITE);
        uint8_t nextQueue[5];
        game.getNextQueue(nextQueue, 5);

        for (int i = 0; i < 5; i++) 
        {
            uint8_t nextPieceIndex = nextQueue[i];
            uint16_t nextShape = CONSTANTS::PIECES[nextPieceIndex][0]; 

            for (int row = 0; row < 4; ++row) 
            {
                for (int col = 0; col < 4; ++col) 
                {
                    if (nextShape & (1 << (15 - (row * 4 + col)))) 
                    {
                        int drawX = uiOffsetX + (col * uiBlockSize);
                        int drawY = uiOffsetY + (i * 5 * uiBlockSize) + (row * uiBlockSize); 
                        
                        DrawRectangle(drawX, drawY, uiBlockSize - 1, uiBlockSize - 1, colors[nextPieceIndex + 1]);
                    }
                }
            }
        }

        int uiHoldOffsetX = boardOffsetX + (10 * blockSize) + 16;
        int uiHoldOffsetY = 360; 

        DrawText("HOLD", uiHoldOffsetX, uiHoldOffsetY, 20, game.canHoldPiece() ? RAYWHITE : GRAY);

        if (game.hasHeldPiece()) 
        {
            uint8_t holdIndex = game.getHeldPieceIndex();
            uint16_t holdShape = CONSTANTS::PIECES[holdIndex][0]; 

            for (int row = 0; row < 4; ++row) 
            {
                for (int col = 0; col < 4; ++col) 
                {
                    if (holdShape & (1 << (15 - (row * 4 + col)))) 
                    {
                        int drawX = uiHoldOffsetX + (col * uiBlockSize);
                        int drawY = (uiHoldOffsetY + uiBlockSize * 2) + (row * uiBlockSize); 
                        
                        Color pieceColor = colors[holdIndex + 1];
                        
                        if (!game.canHoldPiece()) 
                        {
                            pieceColor.r /= 2; 
                            pieceColor.g /= 2; 
                            pieceColor.b /= 2;
                        }

                        DrawRectangle(drawX, drawY, uiBlockSize - 1, uiBlockSize - 1, pieceColor);
                    }
                }
            }
        }

        DrawFPS(5, 5);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}