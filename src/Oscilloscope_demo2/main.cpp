#include <Arduino.h>

int signalPin = 23;  // Input pin
unsigned long highTime, lowTime;
float frequency;

void setup() {
  Serial.begin(115200);
  pinMode(signalPin, INPUT);
}

void loop() {
  highTime = pulseIn(signalPin, HIGH); // microseconds
  lowTime  = pulseIn(signalPin, LOW);  // microseconds

  if (highTime > 0 && lowTime > 0) {
    frequency = 1000000.0 / (highTime + lowTime);
    Serial.println(frequency);
  }
}
