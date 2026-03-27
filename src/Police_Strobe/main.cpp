#include <Arduino.h>

#define LED_RED 22
#define LED_BLUE 23
#define DELAY_TIME 60

void setup(){
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}
void loop(){
    for(int i = 0; i < 3; i++){
        digitalWrite(LED_RED, HIGH);
        delay(DELAY_TIME);
        digitalWrite(LED_RED, LOW);
        delay(DELAY_TIME);
    }
    for(int i = 0; i < 3; i++){
        digitalWrite(LED_BLUE, HIGH);
        delay(DELAY_TIME);
        digitalWrite(LED_BLUE, LOW);
        delay(DELAY_TIME);
    }
} 
