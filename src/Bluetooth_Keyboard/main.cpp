#include <Arduino.h>
#include <BleKeyboard.h>

#define PIN_1 22
#define PIN_2 23

int state1 = 0;
int state2 = 0;

BleKeyboard bleKeyboard;

void setup() {
  pinMode(PIN_1, INPUT_PULLUP);
  pinMode(PIN_2, INPUT_PULLUP);
  Serial.begin(9600);
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()){
    state1 = digitalRead(PIN_1);
    state2 = digitalRead(PIN_2);
    if(state1 == 0){
      bleKeyboard.write('w');
    }
  }
}
