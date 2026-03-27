#include <Arduino.h>

// Motor control pins
#define OUT1 15  // GPIO 15
#define OUT2 2   // GPIO 2
#define OUT3 0   // GPIO 0
#define OUT4 4   // GPIO 4
#define EN1 16   // GPIO 16 (PWM for motor 1)
#define EN2 17   // GPIO 17 (PWM for motor 2)

// Relay pin for 12V water pump
#define RELAY_PIN 18  // GPIO 18 (active-high relay)

// Analog flame sensor pins (ADC1 for ESP32)
#define FLAME_CENTER 34  // GPIO 34 (ADC1_CH6)
#define FLAME_LEFT 35    // GPIO 35 (ADC1_CH7)
#define FLAME_RIGHT 32   // GPIO 32 (ADC1_CH4)

// PWM settings
#define FREQ 5000
#define RESOLUTION 8
#define MOTOR_SPEED 200  // PWM duty cycle (0–255 for 8-bit)

// Calibration constants
#define FORWARD_DURATION 1000  // Time to move forward (ms)
#define TURN_DURATION 300      // Time to turn left/right (ms)
#define STOP_DURATION 200      // Time to stop between movements (ms)
#define FLAME_THRESHOLD 500    // Analog value threshold for flame detection (0–1023)

// Global variables for flame sensor readings
int flameCenter = 1023;  // Initialize to max (no flame)
int flameLeft = 1023;
int flameRight = 1023;

// Function prototypes
void Stop(int interval);
void Forward(int interval);
void Backward(int interval);
void Left(int interval);
void Right(int interval);
void controlWaterPump(bool state);
void readFlameSensors();
void decideDirection();

void setup() {
  // Initialize motor control pins
  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);

  // Initialize relay pin for water pump
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Ensure pump is off at startup

  // Initialize flame sensor pins (analog inputs)
  pinMode(FLAME_CENTER, INPUT);
  pinMode(FLAME_LEFT, INPUT);
  pinMode(FLAME_RIGHT, INPUT);

  // Configure PWM for motor speed control (retaining original ledcAttach)
  ledcAttach(EN1, FREQ, RESOLUTION);
  ledcAttach(EN2, FREQ, RESOLUTION);
  ledcWrite(EN1, MOTOR_SPEED);
  ledcWrite(EN2, MOTOR_SPEED);

  // Configure ADC for 10-bit resolution (0–1023)
  analogReadResolution(10);

  // Start serial communication for debugging
  Serial.begin(115200);
  Stop(0);  // Ensure motors are stopped at startup
}

void loop() {
  // Step 1: Read flame sensor values
  readFlameSensors();

  // Step 2: Debug output
  Serial.print("Center: "); Serial.print(flameCenter);
  Serial.print(" | Left: "); Serial.print(flameLeft);
  Serial.print(" | Right: "); Serial.println(flameRight);

  // Step 3: Decide movement based on flame detection
  decideDirection();

  // Step 4: Brief pause before next cycle
  Stop(STOP_DURATION);
}

// Stop all motors
void Stop(int interval) {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);
  controlWaterPump(false);  // Ensure water pump is off when stopped
  if (interval > 0) delay(interval);
}

// Move forward
void Forward(int interval) {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
  if (interval > 0) delay(interval);
}

// Move backward
void Backward(int interval) {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, HIGH);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, HIGH);
  if (interval > 0) delay(interval);
}

// Turn left (right motor forward only)
void Left(int interval) {
  digitalWrite(OUT1, LOW);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, HIGH);
  digitalWrite(OUT4, LOW);
  if (interval > 0) delay(interval);
}

// Turn right (left motor forward only)
void Right(int interval) {
  digitalWrite(OUT1, HIGH);
  digitalWrite(OUT2, LOW);
  digitalWrite(OUT3, LOW);
  digitalWrite(OUT4, LOW);
  if (interval > 0) delay(interval);
}

// Control the water pump via relay
void controlWaterPump(bool state) {
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  Serial.print("Water pump: ");
  Serial.println(state ? "ON" : "OFF");
}

// Read values from all flame sensors
void readFlameSensors() {
  flameCenter = analogRead(FLAME_CENTER);
  flameLeft = analogRead(FLAME_LEFT);
  flameRight = analogRead(FLAME_RIGHT);
  delay(100);  // Allow stable readings
}

// Decide movement direction based on flame sensor readings
void decideDirection() {
  // Check if any flame is detected (lower values mean stronger flames)
  if (flameCenter < FLAME_THRESHOLD) {
    Serial.println("Flame detected ahead: Moving forward, water pump ON");
    controlWaterPump(true);  // Turn on water pump
    Forward(FORWARD_DURATION);
    controlWaterPump(false); // Turn off water pump
  } else if (flameLeft < FLAME_THRESHOLD && flameLeft < flameRight) {
    Serial.println("Flame detected on left: Turning left, water pump ON");
    Left(TURN_DURATION);
    controlWaterPump(true);  // Turn on water pump
    Forward(FORWARD_DURATION);
    controlWaterPump(false); // Turn off water pump
  } else if (flameRight < FLAME_THRESHOLD) {
    Serial.println("Flame detected on right: Turning right, water pump ON");
    Right(TURN_DURATION);
    controlWaterPump(true);  // Turn on water pump
    Forward(FORWARD_DURATION);
    controlWaterPump(false); // Turn off water pump
  } else {
    Serial.println("No flame detected: Stopping");
    Stop(STOP_DURATION);
  }
}
