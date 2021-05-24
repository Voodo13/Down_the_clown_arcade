#include <Arduino.h>
#include "GyverTimer.h"
#include "GyverButton.h"

struct Game {
    bool begin = false;
    byte ticketRatio = 1;
    byte coins = 0;
    byte points = 0;
} game;

struct Dispensor {
    bool change = false;
    bool dispense = false;
    byte tickets = 0;
} dispensor;

struct Counter {
    int tickets = 0;
} counter;


// ---------- Deep Switch Read --------
void dipSvchRead() {
  DDRC = 0b00000000;
  PORTC = 0b11111111;
  game.ticketRatio = (PINC | 0b00100000) ^ 0b00111111;
  game.ticketRatio = (PINC | 0b00100000) ^ 0b00111111;
}
// ------------------------------------

// ---------- Sensors Read ------------
void sensorSetup() {
  for (byte i = 6; i <= 13; i++) {
    pinMode(i, INPUT_PULLUP);
  }
}

byte sensorsRead() {
  byte counter = 0;
  for (byte i = 6; i <= 13; i++) {
    if(digitalRead(i)) counter ++;
  }
  return counter;
}
// ------------------------------------

// ---------- Counter send ------------
void counterSetup() {
  pinMode(5, OUTPUT);
}

void counterFn() {
  if(!counter.tickets) return;
  digitalWrite(5, 1);
  digitalWrite(5, 0);
  counter.tickets--;
}
// ------------------------------------

// ----------- Dispensor --------------
GButton dsp(2);

void dispensorSetup() {
  dsp.setDebounce(1);
  pinMode(4, OUTPUT);
}

// обработка прерывания
void ticketDec() {
  byte &tickets = dispensor.tickets;
  dispensor.change = true;
  if(!tickets) return;
  tickets--;
}

void dispensorFn() {
  dsp.tick();
  if(dsp.isPress()) ticketDec();

  if(!dispensor.change) return;
  dispensor.change = false;
  digitalWrite(4, (bool)dispensor.tickets);

  // отладка
   Serial.print(" tickets: ");
   Serial.println(dispensor.tickets);
}
// ------------------------------------

// ----------- Coin --------------
GButton cbtn(3);

void coinSetup() {
		cbtn.setDebounce(10);
  //pinMode(3, INPUT_PULLUP);
}

void addCoin() {
  game.coins++;
  // отладка
  Serial.print("coin++ : ");
  Serial.println(game.coins);
}
// ------------------------------------

// ----------- Game --------------
GTimer gameTimer(MS);
byte sensorsCounter = 0;

void gameFn() {
  if((game.coins > 0) && !game.begin) {
    if(sensorsRead()) return;
    game.coins--;
    game.begin = true;
    game.points = 0;
    gameTimer.setTimeout(60000); // время игры mc
    
    // отладка
    Serial.println("Game Start");
  }

  if(game.begin) {
  sensorsCounter = sensorsRead();
  if(sensorsCounter > game.points) game.points = sensorsCounter;
  if(gameTimer.isReady() || game.points == 8) {
    game.begin = false;
    dispensor.tickets += game.points * game.ticketRatio;
    if(dispensor.tickets > 0) {
      dispensor.dispense = true;
      dispensor.change = true;
    }

    // отладка
    Serial.print("Game End - points: ");
    Serial.print(game.points);
    Serial.print(" tickets: ");
    Serial.print(dispensor.tickets);
    Serial.print(" coins: ");
    Serial.println(game.coins);
    }
  }
}
// ------------------------------------

void setup() {
  // чтение дип свича и установка коофициента
  dipSvchRead();  
  // включение прерывания на pin2 для диспенсора
  dispensorSetup();
  // включение прерывания на pin3 для коинов
  coinSetup();
  //attachInterrupt(1, addCoin, FALLING);
  // настройка пинов под сенсоры
  sensorSetup();
  //пины счетчика
  counterSetup();

  // отладка
  Serial.begin(9600);
  Serial.print("Ready! ticketRatio: ");
  Serial.println(game.ticketRatio);
}

void loop() {
	cbtn.tick();
	if(cbtn.isPress()) addCoin();
  gameFn();
  dispensorFn();
  counterFn();
}