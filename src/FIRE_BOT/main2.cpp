// Monday, October 27, 2025 1:46:11 PM Bangladesh Standard Time

#include <Arduino.h>
#include <ESP32Servo.h>

#define OUT1 15
#define OUT2 2
#define OUT3 0
#define OUT4 4
#define EN1 16
#define EN2 17

#define S_PIN 22
#define F_PIN 32

#define TRIG 18
#define ECHO 19

#define FREQ 5000
#define RESOLUTION 8

#define SERVO_SPEED 5 // Note: speed must be a value greater than 0, smaller value means greater speed.

int F_value = 0;
int currentPos_G = 0;
int distance;
int direction;

Servo servo;

void Stop(int interval);
void Forward(int interval);
void Backward(int interval);
void Left(int interval);
void Right(int interval);
int getDistance();
int getDirection();
int angleDelta(int currentPos, int targetPos);

void setup(){
 pinMode(OUT1, OUTPUT);
 pinMode(OUT2, OUTPUT);
 pinMode(OUT3, OUTPUT);
 pinMode(OUT4, OUTPUT);
 ledcAttach(EN1, FREQ, RESOLUTION);
 ledcAttach(EN2, FREQ, RESOLUTION);
 pinMode(F_PIN, INPUT);
 pinMode(TRIG, OUTPUT);
 pinMode(ECHO, INPUT);
 Serial.begin(115200);
 servo.attach(S_PIN);
 delay(500);
 Serial.println("Pins setup done..");
 ledcWrite(EN1, 200);
 ledcWrite(EN2, 200);
 angleDelta(0, 90);
 currentPos_G = 90;
 Serial.println("Servo wrote..");
 Stop(0);
}

void loop(){
   Serial.println("Forwarding..");
   distance = getDistance();
   while(distance > 23){
     Forward(0);
     distance = getDistance();
     delay(50);
   }
   Serial.println("Forwarding ended");
   Stop(0);
   delay(150);
   direction = getDirection();
   if(direction == 0){
     distance = getDistance();
     if(distance > 23){
       Serial.println("Left");
       Left(333);
     }else{
       Serial.println("Stopped");
       Stop(500);
       Backward(1000);
     }
   }else if(direction == 1){
     distance = getDistance();
     if(distance > 23){
       Serial.println("Right");
       Right(333);
     }else{
       Serial.println("Stopped");
       Stop(500);
       Backward(1000);
     }
   }
}

void Stop(int interval){
 digitalWrite(OUT1, LOW);
 digitalWrite(OUT2, LOW);
 digitalWrite(OUT3, LOW);
 digitalWrite(OUT4, LOW);
 if(interval > 0) delay(interval);
}

void Forward(int interval){
 digitalWrite(OUT1, HIGH);
 digitalWrite(OUT2, LOW);
 digitalWrite(OUT3, HIGH);
 digitalWrite(OUT4, LOW);
 if(interval > 0) delay(interval);
}

void Backward(int interval){
 digitalWrite(OUT1, LOW);
 digitalWrite(OUT2, HIGH);
 digitalWrite(OUT3, LOW);
 digitalWrite(OUT4, HIGH);
 if(interval > 0) delay(interval);
}

void Left(int interval){
 digitalWrite(OUT1, LOW);
 digitalWrite(OUT2, LOW);
 digitalWrite(OUT3, HIGH);
 digitalWrite(OUT4, LOW);
 if(interval > 0) delay(interval);
}

void Right(int interval){
 digitalWrite(OUT1, HIGH);
 digitalWrite(OUT2, LOW);
 digitalWrite(OUT3, LOW);
 digitalWrite(OUT4, LOW);
 if(interval > 0) delay(interval);
}

int getDistance(){
 int duration = 0;
 int distance = 0;
 digitalWrite(TRIG, LOW);
 delayMicroseconds(2);
 digitalWrite(TRIG, HIGH);
 delayMicroseconds(10);
 digitalWrite(TRIG, LOW);
 duration = pulseIn(ECHO, HIGH, 30000);
 distance = duration*0.03313/2;
 if(distance <= 0 || distance > 220){
  Serial.println("Distance measuring failed.");
  return -1;
 }else{
  Serial.print("Distance measuring succedd: ");
  Serial.println(distance);
  return distance;
 }
}

int getDirection(){
  int left = 0;
  int right = 0;
  currentPos_G = angleDelta(currentPos_G, 0);
  delay(200);
  left = getDistance();
  currentPos_G = angleDelta(currentPos_G, 180);
  delay(200);
  right = getDistance();
  if(left > right){
    Serial.println("Left Decided");
    return 0;
  }else{
    Serial.println("Right Decided");
    return 1;
  }
}

int angleDelta(int currentPos, int targetPos){
  if(targetPos > currentPos){
    for(int i = currentPos+1; i <= targetPos; i++){
      servo.write(i);
      delay(SERVO_SPEED);
    }
  }else if(currentPos > targetPos){
    for(int i = currentPos-1; i >= targetPos; i--){
      servo.write(i);
      delay(SERVO_SPEED);
    }
  }else if(currentPos == targetPos){
    servo.write(targetPos);
  }
  Serial.println("angleDelta done");
  return targetPos;
}
