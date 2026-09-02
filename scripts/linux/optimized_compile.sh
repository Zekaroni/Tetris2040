g++ main.cpp src/GameLogic.cpp src/GameController.cpp \
    -o tetramino \
    -Os -s \
    -ffunction-sections -fdata-sections \
    -Wl,--gc-sections \
    -fno-exceptions -fno-rtti -flto \
    $(pkg-config --cflags --libs raylib)
