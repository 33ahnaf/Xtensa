#include <Arduino.h>

#define L1 13
#define L2 12
#define L3 14
#define L4 27
#define L5 26
#define L6 25
#define L7 33
#define L8 32
#define L9 22
#define L10 23

int LED_PINs[10] = {L1, L2, L3, L4, L5, L6, L7, L8, L9, L10};

void setup(){
  pinMode(LED_PINs[0], OUTPUT);
  pinMode(LED_PINs[1], OUTPUT);
  pinMode(LED_PINs[2], OUTPUT);
  pinMode(LED_PINs[3], OUTPUT);
  pinMode(LED_PINs[4], OUTPUT);
  pinMode(LED_PINs[5], OUTPUT);
  pinMode(LED_PINs[6], OUTPUT);
  pinMode(LED_PINs[7], OUTPUT);
  pinMode(LED_PINs[8], OUTPUT);
  pinMode(LED_PINs[9], OUTPUT);
}

void loop(){
  for(int i = 0; i < 10; i++){
    for(int j = 0; j <= 255; j++){
      analogWrite(LED_PINs[i], j);
      // delay(1);
      delayMicroseconds(78);
    }
    // digitalWrite(LED_PINs[i], 0);
    delay(50);
  }
  for(int i = 0; i < 10; i++){
    for(int j = 255; j >= 0; j--){
      analogWrite(LED_PINs[i], j);
      // delay(1);
      delayMicroseconds(78);
    }
    // digitalWrite(LED_PINs[i], 1);
    delay(50);
  }
}
