#include <Arduino.h>

struct Game {
    bool timer = false;
    bool begin = false;
    byte ticketRatio = 1;
    byte coins = 0;
    byte points = 0;
    int time = 60;
} game;

struct Dispensor {
    bool change = false;
    bool dispense = false;
    int tickets = 0;
} dispensor;

struct Counter {
    int tickets = 0;
} counter;