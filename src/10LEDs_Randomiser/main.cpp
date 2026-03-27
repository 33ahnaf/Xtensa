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
#define DELAY_TIME 100

int LED_PINs[10] = {L1, L2, L3, L4, L5, L6, L7, L8, L9, L10};

void setup(){
  pinMode(15, INPUT);
  randomSeed(analogRead(15));
  
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);
  pinMode(L4, OUTPUT);
  pinMode(L5, OUTPUT);
  pinMode(L6, OUTPUT);
  pinMode(L7, OUTPUT);
  pinMode(L8, OUTPUT);
  pinMode(L9, OUTPUT);
  pinMode(L10, OUTPUT);
  for(int i = 0; i < 10; i++){
    digitalWrite(LED_PINs[i], 0);
  }
}

void loop(){
  for(int i = 0; i < 10; i++){
    digitalWrite(LED_PINs[i], random(2));
  }
  delay(DELAY_TIME);
}
