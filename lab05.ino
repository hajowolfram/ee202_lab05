//
// Felix Yang, Hajo Wolfram
// EE Lab 5 2.3
//

// pin declarations
const int clkA = 32;
const int datA = 33;
const int rstA = 25;
const int clkB = 19;
const int datB = 18;
const int rstB = 17;

// row tracker
volatile int curr_row = 0;

// timer
uint32_t delay_time_us = 1000000;

// initialize timer
hw_timer_t* led_timer = NULL;
portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;

uint8_t mat[3][3] = {
    {1, 0, 0},
    {0, 1, 1},
    {1, 0, 1}
};

// shift bits
void shift_one_bit_a(uint8_t d) {
    digitalWrite(datA, d);
    digitalWrite(clkA, HIGH);
    digitalWrite(clkA, LOW);
}

void shift_one_bit_b(uint8_t d) {
    digitalWrite(datB, d);
    digitalWrite(clkB, HIGH);
    digitalWrite(clkB, LOW);
}


// reset_matrix
void reset_matrix() {
  for (int i = 0; i < 3; i++) { // PMOS
    shift_one_bit_a(1);
  }
  digitalWrite(rstB, HIGH); // NMOS
  digitalWrite(rstB, LOW);
}

void IRAM_ATTR animation_handler() {
    portENTER_CRITICAL_ISR(&timer_mux);

    // turn off other rows
    // TODO: potentially optimize this?
    reset_matrix();
    for (uint8_t row = 0; row < curr_row; ++row) {
        shift_one_bit_a(HIGH);
    }

    // only enable current row (pmos->LOW)
    shift_one_bit_a(LOW);

    // update curr_row
    if (curr_row == 2) {
        curr_row = 0;
    }
    else {
        curr_row++;
    }

    // load cols
    // 3 cols hard-coded
    for (uint8_t col = 0; col < 3; ++col) {
        uint8_t status = mat[curr_row][2 - col];
        shift_one_bit_b(status);
    }

    portEXIT_CRITICAL_ISR(&timer_mux);
}

// accepts array of columns, updates pattern matrix.
// each col entry indicates number of LEDs to light from bottom
// i.e. cols = [1, 2, 3] 
//  0 0 *
//  0 * *
//  * * *
void update_mat(uint8_t* cols, uint8_t num_rows, uint8_t num_cols) {
    // iterate thru cols array
    // do we need to reset matrix first?
    for (int idx = 0; idx < num_cols; ++idx) {
        // iterate thru rows, lighting up appropriate number of LEDs
        for (int j = 0; j < cols[idx]; ++j) {
            mat[num_rows - j - 1][idx] = 1;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(clkA, OUTPUT);
    pinMode(datA, OUTPUT);
    pinMode(rstA, OUTPUT);
    pinMode(clkB, OUTPUT);
    pinMode(datB, OUTPUT);
    pinMode(rstB, OUTPUT);

    // set timer freq to 1MHz
    led_timer = timerBegin(1000000);
    timerAttachInterrupt(led_timer, &animation_handler);

    // timer triggers every second
    // true -> timer should repeat
    timerAlarm(led_timer, delay_time_us, true, 0);
}

void loop()
{
    static uint8_t cols[3];

    cols[0] = 1; cols[1] = 2; cols[2] = 1;
    update_mat(cols, 3, 3);
    delay(1000);
    cols[0] = 2; cols[1] = 1; cols[2] = 3;
    update_mat(cols, 3, 3);
    delay(1000);
}