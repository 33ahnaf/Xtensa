#include <Arduino.h>
#include <math.h>
#define TIMEOUT 10
#define TRUE 1
#define FALSE 0

int isPrime(unsigned long long);

void setup(){
  setCpuFrequencyMhz(240);
  Serial.begin(9600);
}

void loop(){
  unsigned long long i = 0;
  int count = 0;
  int init = millis();
  while(millis() - init < (TIMEOUT * 1000)){
    if(isPrime(i)) count++;
      i++;
  }
  printf("%d\n", count);
}

int isPrime(unsigned long long num){
  if(num < 2) return 0;
  if(num == 2) return 1;
  if(num % 2 == 0) return 0;
  unsigned long long root_over_num = sqrt(num);
  for(unsigned long long i = 2; i <= root_over_num; i++){
    if(num % i == 0) return 0;
  }
  return 1;
}
