@echo off
g++ -std=c++17 ./tests/test.cpp ./src/GameLogic.cpp -Iinclude -o ./tests/test
"./tests/test"