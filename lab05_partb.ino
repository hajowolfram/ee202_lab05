#include <Arduino.h>
const int clkA = 32;
const int datA = 33;
const int rstA = 25;
const int clkB = 19;
const int datB = 18;
const int rstB = 17;

volatile uint8_t curr_row = 0;

uint32_t delay_time_us = 2000;  // 1 second in microseconds
hw_timer_t* led_timer = nullptr;
portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;

uint8_t mat[3][3] = {
  {1, 0, 0},
  {0, 1, 1},
  {1, 0, 1}
};

inline void reset_matrix() {
  for (int i = 0; i < 3; i++) {
    shift_one_bit_a(1);
  }
  digitalWrite(rstB, HIGH);
  digitalWrite(rstB, LOW);
}

inline void shift_one_bit_a(uint8_t d) {
  digitalWrite(datA, d);
  digitalWrite(clkA, HIGH);
  digitalWrite(clkA, LOW);
}

inline void shift_one_bit_b(uint8_t d) {
  digitalWrite(datB, d);
  digitalWrite(clkB, HIGH);
  digitalWrite(clkB, LOW);
}

void IRAM_ATTR animation_handler() {
  portENTER_CRITICAL_ISR(&timer_mux);
  reset_matrix();
  curr_row = (curr_row + 1) % 3;
  shift_one_bit_a(LOW);
  for (uint8_t r = 0; r < curr_row; r++) {
    shift_one_bit_a(HIGH);
  }
  for (uint8_t col = 0; col < 3; col++) {
    shift_one_bit_b(mat[curr_row][2 - col]);
  }
  portEXIT_CRITICAL_ISR(&timer_mux);
}

void update_mat(const uint8_t* cols, uint8_t num_rows, uint8_t num_cols) {
  for (uint8_t r = 0; r < num_rows; r++) {
    for (uint8_t c = 0; c < num_cols; c++) {
      mat[r][c] = 0;
    }
  }

  for (uint8_t c = 0; c < num_cols; c++) {
    for (uint8_t j = 0; j < cols[c] && j < num_rows; j++) {
      mat[num_rows - 1 - j][c] = 1;
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
  digitalWrite(rstA, LOW);
  digitalWrite(rstB, LOW);

  led_timer = timerBegin(1000000);
  timerAttachInterrupt(led_timer, &animation_handler);
  timerAlarm(led_timer, delay_time_us, true, 0);
}

void loop() {
  static uint8_t cols[3];

  cols[0] = 1; cols[1] = 2; cols[2] = 1;
  update_mat(cols, 3, 3);
  delay(1000);
  cols[0] = 2; cols[1] = 1; cols[2] = 3;
  update_mat(cols, 3, 3);
  delay(1000);
 }
