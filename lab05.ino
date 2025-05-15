// ==== Section 2.2: Basic Scanning Template ====

#include <Arduino.h>

// pin declarations
const int clkA      = XX;
const int da        = XX;
const int rstA      = XX;
const int clkB      = XX;
const int db        = XX;
const int rstB      = XX;

// delay between LEDs in microseconds (default 1 sec)
uint32_t delayTimeUs = 1'000'000;

void resetMatrix() {
  // reset the matrix by turning off all transistors.
  // row-control transistors are pMOS, so set them HIGH; 
  // column-control are nMOS, so set them LOW.
  digitalWrite(rstA, HIGH);
  digitalWrite(rstB, HIGH);
}

void shiftOneBitA(uint8_t d) {
  // shift single bit d into register A
  digitalWrite(da, d);
  digitalWrite(clkA, HIGH);
  digitalWrite(clkA, LOW);
}

void shiftOneBitB(uint8_t d) {
  // shift single bit d into register B
  digitalWrite(db, d);
  digitalWrite(clkB, HIGH);
  digitalWrite(clkB, LOW);
}

// check serial input to modify the delay parameter
void checkInput() {
  while (Serial.available() > 0) {
    delayTimeUs = Serial.parseInt();
    Serial.println(delayTimeUs);
  }
}

void setup() {
  // enable serial communication
  Serial.begin(115200);

  // initialize pins
  pinMode(clkA, OUTPUT);
  pinMode(da,   OUTPUT);
  pinMode(rstA, OUTPUT);
  pinMode(clkB, OUTPUT);
  pinMode(db,   OUTPUT);
  pinMode(rstB, OUTPUT);

  // default outputs
  digitalWrite(clkA, LOW);
  digitalWrite(da,   LOW);
  digitalWrite(rstA, HIGH);
  digitalWrite(clkB, LOW);
  digitalWrite(db,   LOW);
  digitalWrite(rstB, HIGH);

  // reset the matrix
  resetMatrix();
}

void loop() {
  checkInput();

  // Example: light one LED at a time by
  // iterating rows and columns with delayMicroseconds.
  for (uint8_t row = 0; row < 3; ++row) {
    // set up row (not shown)
    for (uint8_t col = 0; col < 3; ++col) {
      // shift pattern into A and B for (row,col)
      shiftOneBitA((col == row) ? 1 : 0);
      shiftOneBitB((col == row) ? 1 : 0);
      delayMicroseconds(delayTimeUs);
    }
  }
}


