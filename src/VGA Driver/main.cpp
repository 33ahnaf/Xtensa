#include <Arduino.h>
#include <ESP32Video.h>
#include <Ressources/Font6x8.h>

const int outputPin = 25;
int x = 0, y = 0;

CompositeGrayDAC display;

void setup(){
  Serial.begin(9600);
  display.init(CompMode::MODEPAL288P, 25, false);
  display.setFont(Font6x8);
}

void loop(){
  if(Serial.available() != 0){
    if(Serial.read() == 'x'){
      x = Serial.parseInt();
    }else if(Serial.read() == 'y'){
      y = Serial.parseInt();
    }
  }
  display.clear();
  display.setCursor(x, y);
  display.print("Hello, world!");
  Serial.print(x);
  Serial.print("\t\t");
  Serial.println(y);
  delay(20);
}