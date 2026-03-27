#include <Arduino.h>





// Flame sensor analog pins
#define FLAME_LEFT   32
#define FLAME_CENTER 33
#define FLAME_RIGHT  34

// Motor control pins
#define OUT1 15
#define OUT2 2
#define OUT3 0
#define OUT4 4
#define EN1 16
#define EN2 17

// Water pump relay pin
#define PUMP_PIN 22

// PWM settings
#define FREQ 5000
#define RESOLUTION 8

// Flame detection threshold
#define FLAME_THRESHOLD 3000

String fireStatus = "Safe";
bool manualPumpControl = false;

void stopMotors() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);
}

void forward() {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
  delay(500);
  stopMotors();
}

void backward() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, HIGH);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, HIGH);
  delay(500);
  stopMotors();
}

void left() {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
  delay(500);
  stopMotors();
}

void right() {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);
  delay(500);
  stopMotors();
}

void setup() {
  Serial.begin(115200);

  // Motor setup
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  ledcAttach(EN1, FREQ, RESOLUTION);
  ledcAttach(EN2, FREQ, RESOLUTION);
  ledcWrite(EN1, 200);
  ledcWrite(EN2, 200);

  // Flame sensors
  pinMode(FLAME_LEFT, INPUT);
  pinMode(FLAME_CENTER, INPUT);
  pinMode(FLAME_RIGHT, INPUT);

  // Pump relay
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Pump off initially

  Serial.println("ESP32 ready. Type F, B, L, R, STOP, STATUS, PUMP ON, PUMP OFF");
}

void loop() {
  // Fire detection
  int leftVal = analogRead(FLAME_LEFT);
  int centerVal = analogRead(FLAME_CENTER);
  int rightVal = analogRead(FLAME_RIGHT);

  bool fireDetected = leftVal < FLAME_THRESHOLD || centerVal < FLAME_THRESHOLD || rightVal < FLAME_THRESHOLD;

//  if (!manualPumpControl) {
//    if (fireDetected) {
//      fireStatus = "🔥 Fire Detected!";
//      digitalWrite(PUMP_PIN, HIGH);
//    } else {
//      fireStatus = "✅ Safe";
//      digitalWrite(PUMP_PIN, LOW);
//    }
//  }

  // Serial command handling
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "F") {
      forward();
      digitalWrite(PUMP_PIN, LOW);
      Serial.println("Moving Forward");
    } else if (cmd == "B") {
      backward();
      Serial.println("Moving Backward");
    } else if (cmd == "L") {
      left();
      Serial.println("Turning Left");
    } else if (cmd == "R") {
      right();
      Serial.println("Turning Right");
    } else if (cmd == "STOP") {
      stopMotors();
      Serial.println("Motors Stopped");
    }
//     else if (cmd == "STATUS") {
//      Serial.println("Fire Status: " + fireStatus);
//      Serial.printf("Flame values: L=%d C=%d R=%d\n", leftVal, centerVal, rightVal);
//    } else if (cmd == "PUMP ON") {
////      manualPumpControl = true;
//      digitalWrite(PUMP_PIN, HIGH);
//      Serial.println("Pump turned ON manually");
//    } else if (cmd == "PUMP OFF") {
//      manualPumpControl = true;
//      digitalWrite(PUMP_PIN, LOW);
//      Serial.println("Pump turned OFF manually");
//    } else if (cmd == "PUMP AUTO") {
//      manualPumpControl = false;
//      Serial.println("Pump set to AUTO mode (fire detection)");
      else {
      Serial.println("Unknown command");
      digitalWrite(PUMP_PIN, HIGH);
    }
  }

  delay(100);
}

// Movement functions
