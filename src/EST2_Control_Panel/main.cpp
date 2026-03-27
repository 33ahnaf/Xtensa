#include <Arduino.h>

int pins[22] = {15, 2, 4, 16, 17, 5, 18, 19, 21, 22, 23, 32, 33, 25, 26, 27};
char keys[] = {'s', '^', '[', ']', 'f', 'h', 'n', 'e', '!', 'u', 'v', 'l', 'j', 'o', 'p', 't'};

void setup(){
  for(int i = 0; i < 16; i++){
    pinMode(pins[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
}

void loop(){
  for(int i = 0; i < 16; i++){
    if(digitalRead(pins[i]) == 0){
      Serial.println(keys[i]);
      delay(200);
    }
  }
}
