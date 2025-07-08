# Tetris2040
This is a project that runs Tetris on a Pico RP2040.

## Background
I ([Ezekiel Hall](https://www.linkedin.com/in/zekehall/)) started this project in 2023 when I decided to try to model a [DAC](https://en.wikipedia.org/wiki/Digital-to-analog_converter) (digital to analoug converter) that ran off the Pico RP2040. I made the circuit for a DAC and then wrote code to produce [square](https://en.wikipedia.org/wiki/Square_wave_(waveform)), [triangle](https://en.wikipedia.org/wiki/Triangle_wave), [sawtooth](https://en.wikipedia.org/wiki/Sawtooth_wave), and other forms of waves to produce audio with this DAC. After programming a Tetris demo for this soundcard, and actively following [Ben Eater's series](https://youtu.be/l7rce6IQDWs) on making his own GPU, I decided it would be fun to try to make my own Tetris game where I have develop every part of machine besides the [RP2040 processor](https://en.wikipedia.org/wiki/RP2040). I am now finally revisiting this project in 2025 in hopes that I am able to apply the various things I have learned over the past few years coding and attending college. I hope to use this project as my capstone for graduation.

## The Plan
I plan to only use one RP2040 if possible. The RP2040 has two [cores](https://en.wikipedia.org/wiki/ARM_Cortex-M#Cortex-M0+) that run at 133MHz and 2 [PIO](https://tutoduino.fr/en/pio-rp2040-en/) blocks that each have 4 state machines. The PICO has about 260KB of RAM and 2MB of program memory.

## Structure
The main structure I plan to follow is:
```
Tetris2040/
├── src/
│   ├── main.cpp
│   ├── Input.cpp
│   ├── GameLogic.cpp
│   ├── VGADriver.cpp
│   ├── AudioDriver.cpp
├── include/
│   ├── Input.h
│   ├── GameLogic.h
│   ├── VGADriver.h
│   ├── AudioDriver.h
│   └── Constants.h
├── lib/
├── build/
├── docs/
└── .gitignore

```

# Research
## Tetris Rules
I started by modeling the game after the [offical rules](https://tetris.wiki/Tetris_Guideline) first, giving me a nice base to work with and the ability to start adding on community rules later if I wanted to.

### Rotation
![image](https://i.sstatic.net/UbPC9.png)
(source: https://stackoverflow.com/questions/74340309/tetris-rotation-representation)
### Bag Generation
A "bag" in Tetris referres to a shuffled group of tetriminos that are usually shuffled in pairs of 7 or 14 to allow for randomized sequences of the pieces that aren't repetitive. The _[Random Generator](https://tetris.wiki/Random_Generator)_ is the standard bag randomizer. This software uses a custom 7 piece bag randomization that has a queue of 14 for the ability to always have piece _"look ahead"_.

## Advance Programming Concepts

### Bitmaps or Bitsets or Bitarrays
[Bitmaps](https://www.tutorialspoint.com/what-is-bitmap) are something I learned about a long time ago when watching Sebastian Lague's YouTube channel where he does various coding projects under the name "Coding Adventures". More specifically, his chess series is where I drew my main inspiration for using bitmaps for effiecency in memory and compute time. The way I implement these maps will be with the built-in functionality of [std::bitset](https://en.cppreference.com/w/cpp/utility/bitset.html), allowing me to define the exact size I need and take advantage of the already avalible functions.

## Testing
### Emulation
#### Game Logic
- Testing game logic was implemented by creating a separate renderer for _Windows_ that could run the game logic and wrap the output to render the Tetris board for more effiecnt testing without needing to compile for and render form the RP2040.