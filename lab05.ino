#include <Arduino.h>

const int clkA = 32;
const int datA = 33;
const int rstA = 25;
const int clkB = 19;
const int datB = 18;
const int rstB = 17;

volatile uint32_t DELAY_TIME_US = 1000000;

inline void resetMatrix() {
  for (int i = 0; i < 3; i++) { // PMOS
    shiftOneBit(clkA, datA, 1);
  }
  digitalWrite(rstB, HIGH); // NMOS
  digitalWrite(rstB, LOW);
}

inline void shiftOneBit(int clk, int pin, uint8_t d) {
  digitalWrite(pin, d);
  digitalWrite(clk, HIGH);
  digitalWrite(clk, LOW);
}

inline void checkInput() {
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    if (input > 0) {
      DELAY_TIME_US = Serial.parseInt();
      Serial.println(DELAY_TIME_US);
    } else {
      Serial.println("Invalid input");
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(clkA, OUTPUT);
  pinMode(datA, OUTPUT);
  pinMode(rstA, OUTPUT);
  pinMode(clkB, OUTPUT);
  pinMode(datB, OUTPUT);
  pinMode(rstB, OUTPUT);
  resetMatrix();
}

void loop() {
  resetMatrix();
  checkInput();

  for (int row = 0; row < 3; row++) {
    shiftOneBit(clkA, datA, row == 0 ? 0 : 1);
    shiftOneBit(clkB, datB, 1);
    delayMicroseconds(DELAY_TIME_US);
    for (int col = 0; col < 2; col++) {
      shiftOneBit(clkB, datB, 0);
      delayMicroseconds(DELAY_TIME_US);
    }
  }
}
