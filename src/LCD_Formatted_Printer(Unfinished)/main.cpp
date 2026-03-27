#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdint.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

void lprint(char *input, uint8_t x, uint8_t y);
void lprinta(char *input);
void slice(const char *input, char *output, uint8_t s_index, uint8_t e_index);





void setup(){
  Wire.begin(22, 23);
  lcd.begin();
}

void loop(){
  lprinta("Ahnaf and Arfina are the best friends ever.");
  delay(1000);
}







void lprint(char *input, uint8_t x, uint8_t y){
  lcd.clear();
  lcd.setCursor(x, y);
  lcd.print(input);
}

void lprinta(char *line1){
  char *line2, *line3, *line4;
  lcd.clear();
  if(sizeof(line1) <= 17){
    lcd.setCursor(0, 0);
    lcd.print(line1);
  }else if(sizeof(line1) <= 34){
    slice(line1, line2, 17, 33);
    slice(line1, line1, 0, 16);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }else if(sizeof(line1) <= 42){
    slice(line1, line2, 17, 33);
    slice(line1, line3, 34, 41);
    slice(line1, line1, 0, 16);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lcd.setCursor(0, 2);
    lcd.print(line3);
  }else if(sizeof(line1) <= 50){
    slice(line1, line2, 17, 33);
    slice(line1, line3, 34, 41);
    slice(line1, line4, 42, 49);
    slice(line1, line1, 0, 16);
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lcd.setCursor(0, 2);
    lcd.print(line3);
    lcd.setCursor(0, 3);
    lcd.print(line4);
  }
}

void slice(const char *input, char *output, uint8_t s_index, uint8_t e_index){
  uint8_t i = 0;
  for(uint8_t j = s_index; j < e_index + 1; j++){
    output[i] = input[j];
    i++;
  }
  output[i] = '\0';
}
