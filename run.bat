@echo off
g++ main.cpp src/GameLogic.cpp src/GameController.cpp -o tetrimino.exe -lraylib -lopengl32 -lgdi32 -lwinmm
./tetrimino.exe