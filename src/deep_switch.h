#include <Arduino.h>

byte dipSvchRead() {
    DDRC = 0b00000000;
    PORTC = 0b11111111;
    return (PINC | 0b00100000) ^ 0b00111111;
}