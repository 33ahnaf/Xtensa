#include <Arduino.h>
#include <ESP32Video.h>
#include <Ressources/Font6x8.h>

const int outputPin = 25;

CompositeGrayDAC display;

void setup(){
  display.init(CompMode::MODEPAL288P, 25, false);
  display.setFont(Font6x8);
}

void loop(){
  // display.clear();
  // display.setCursor(100, 100);
  // display.print("I LOVE YOU Meghnaaa...");
  // delay(1000);
  // display.clear();
  // display.setCursor(100, 100);
  // display.print("I LOVE YOU Afrinaaa...");
  // delay(1000);
  for(int i = 3; i <= 33; i++){
    display.clear();
    display.setCursor(100, 100);
    display.print("Count: ");
    display.print(i);
    delay(250);
  }
}
