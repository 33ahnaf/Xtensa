#include <Arduino.h>

// 1-D Acoustic Levitator driver for two opposing 40 kHz transducers
// Pins: OC1A = D9, OC1B = D10 (Arduino UNO/Nano)
// Generates 40 kHz square waves with 180° phase difference using Timer1.
// Default: continuous. Optional: burst mode via BURST_MS / GAP_MS.

const bool USE_BURSTS = false;   // set true for burst operation
const uint16_t BURST_MS = 5;     // "on" time (ms) if bursts enabled
const uint16_t GAP_MS   = 50;    // "off" time (ms) if bursts enabled

void start40kHz() {
  // Stop Timer1
  TCCR1A = 0; TCCR1B = 0;
  TCNT1  = 0;

  // Fast PWM, TOP = ICR1 (Mode 14: WGM13:0 = 1110)
  // f_pwm = f_clk / (N * (1 + ICR1))
  // For 40 kHz @ 16 MHz, N = 1 -> ICR1 = 399
  ICR1 = 399;

  // 50% duty on both channels
  OCR1A = (ICR1 + 1) / 2;   // 200
  OCR1B = (ICR1 + 1) / 2;   // 200

  // OC1A non-inverting (COM1A1:0 = 10), OC1B inverting (COM1B1:0 = 11)
  // This yields complementary outputs = 180° phase shift at 50% duty.
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << COM1B1) | (1 << COM1B0)
         | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);  // prescaler = 1

  // Pins 9 & 10 to output
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void stop40kHz() {
  // Disconnect OC1A/OC1B and stop timer (outputs go low)
  TCCR1A = 0;
  TCCR1B = 0;
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void setup() {
  start40kHz();
}

void loop() {
  if (USE_BURSTS) {
    delay(BURST_MS);
    stop40kHz();
    delay(GAP_MS);
    start40kHz();
  } else {
    // continuous wave
  }
}
