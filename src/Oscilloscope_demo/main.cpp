#include <Arduino.h>

#define ADC_PIN 33
const int NUM_SAMPLES = 200;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetPinAttenuation(ADC_PIN, ADC_11db); // 0-3.3V range
}

void loop() {
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int val = analogRead(ADC_PIN);
    Serial.println(val);
  }
}
