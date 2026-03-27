/* =========================================================
   🔥 FireBot - ESP32 (Arduino Core 3.x)
   Saad Bin Ashraf Edition
   Features:
   - 4WD Motor control via L298N
   - Servo scanning using LEDC (no Servo.h)
   - Flame detection with escape behavior
   ========================================================= */

#include <Arduino.h>

// ---------- Pin assignments ----------
#define IN1 15   // Left forward
#define IN2 2    // Left backward
#define IN3 0    // Right forward
#define IN4 4    // Right backward
#define ENA 16   // Left PWM enable
#define ENB 17   // Right PWM enable
#define FLAME_SENSOR 22
#define SERVO_PIN 23

// ---------- LEDC channels ----------
const int CH_LEFT  = 0;
const int CH_RIGHT = 1;
const int CH_SERVO = 2;

// ---------- PWM settings ----------
const int PWM_FREQ_MOT   = 1000;  // Hz
const int PWM_RES_MOT    = 8;     // bits (0–255)
const int PWM_FREQ_SERVO = 50;    // Hz
const int PWM_RES_SERVO  = 16;    // bits

// ---------- Behavior variables ----------
int servoAngle = 90;
int servoDir   = 1;
unsigned long prevMillis = 0;
const unsigned long SCAN_INTERVAL = 40;  // ms

int defaultSpeed = 200;
bool autoMode = true;

// ---------- Escape timings ----------
const unsigned long REVERSE_MS = 700;
const unsigned long TURN_MS    = 650;
const unsigned long PAUSE_AFTER_ESCAPE_MS = 300;

// ---------- Function prototypes ----------
void setupPins();
void setMotorLeft(bool forward, int speed);
void setMotorRight(bool forward, int speed);
void moveForward(int speed);
void moveBackward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void stopBot();
void servoWrite(int angle);
void servoScan();
void patrolMode();
void handleFlame();
void serialHandler();

// =========================================================
//                        SETUP
// =========================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  setupPins();

  servoWrite(servoAngle);  // center servo
  Serial.println("\n🔥 FireBot (ESP32 Core 3.x Ready)");
  Serial.println("Commands: F,B,L,R,S,A (A = auto patrol)");
}

// =========================================================
//                        LOOP
// =========================================================
void loop() {
  serialHandler();

  bool fireDetected = (digitalRead(FLAME_SENSOR) == LOW);

  if (fireDetected) {
    Serial.println("🔥 FIRE DETECTED!");
    handleFlame();
  } else if (autoMode) {
    patrolMode();
  }
}

// =========================================================
//                   PIN + LEDC SETUP
// =========================================================
void setupPins() {
  pinMode(IN1, OUTPUT); digitalWrite(IN1, LOW);
  pinMode(IN2, OUTPUT); digitalWrite(IN2, LOW);
  pinMode(IN3, OUTPUT); digitalWrite(IN3, LOW);
  pinMode(IN4, OUTPUT); digitalWrite(IN4, LOW);
  pinMode(FLAME_SENSOR, INPUT);

  // --- LEDC configuration (Arduino Core 3.x syntax) ---
  ledcAttachChannel(ENA, PWM_FREQ_MOT, PWM_RES_MOT, CH_LEFT);
  ledcAttachChannel(ENB, PWM_FREQ_MOT, PWM_RES_MOT, CH_RIGHT);
  ledcAttachChannel(SERVO_PIN, PWM_FREQ_SERVO, PWM_RES_SERVO, CH_SERVO);

  stopBot();
  ledcWrite(CH_LEFT, 0);
  ledcWrite(CH_RIGHT, 0);
}

// =========================================================
//                       MOTORS
// =========================================================
void setMotorLeft(bool forward, int speed) {
  digitalWrite(IN1, forward ? HIGH : LOW);
  digitalWrite(IN2, forward ? LOW : HIGH);
  ledcWrite(CH_LEFT, constrain(speed, 0, 255));
}

void setMotorRight(bool forward, int speed) {
  digitalWrite(IN3, forward ? HIGH : LOW);
  digitalWrite(IN4, forward ? LOW : HIGH);
  ledcWrite(CH_RIGHT, constrain(speed, 0, 255));
}

void moveForward(int speed) {
  setMotorLeft(true, speed);
  setMotorRight(true, speed);
}

void moveBackward(int speed) {
  setMotorLeft(false, speed);
  setMotorRight(false, speed);
}

void turnLeft(int speed) {
  setMotorLeft(false, speed);
  setMotorRight(true, speed);
}

void turnRight(int speed) {
  setMotorLeft(true, speed);
  setMotorRight(false, speed);
}

void stopBot() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  ledcWrite(CH_LEFT, 0);
  ledcWrite(CH_RIGHT, 0);
}

// =========================================================
//                       SERVO
// =========================================================
void servoWrite(int angle) {
  angle = constrain(angle, 0, 180);
  const float minPulse = 500.0;   // µs
  const float maxPulse = 2500.0;  // µs
  float pulse = minPulse + ((float)angle / 180.0) * (maxPulse - minPulse);

  uint32_t maxDuty = (1UL << PWM_RES_SERVO) - 1;
  uint32_t duty = (uint32_t)((pulse / 20000.0) * maxDuty);
  ledcWrite(CH_SERVO, duty);
}

void servoScan() {
  unsigned long now = millis();
  if (now - prevMillis < SCAN_INTERVAL) return;
  prevMillis = now;

  servoAngle += servoDir;
  if (servoAngle >= 120 || servoAngle <= 60) servoDir = -servoDir;

  servoWrite(servoAngle);
}

// =========================================================
//                   PATROL / FLAME ACTION
// =========================================================
void patrolMode() {
  moveForward(defaultSpeed);
  servoScan();
}

void handleFlame() {
  stopBot();
  delay(100);

  Serial.println("⏪ Reversing...");
  moveBackward(defaultSpeed);
  delay(REVERSE_MS);

  stopBot();
  delay(80);

  Serial.println("↪ Turning...");
  turnRight(defaultSpeed);
  delay(TURN_MS);

  stopBot();
  delay(PAUSE_AFTER_ESCAPE_MS);

  Serial.println("✅ Escape complete. Patrol resumed.");
}

// =========================================================
//                   SERIAL CONTROL
// =========================================================
void serialHandler() {
  if (!Serial.available()) return;

  char c = toupper(Serial.read());
  switch (c) {
    case 'F': autoMode = false; moveForward(defaultSpeed); Serial.println("Manual: Forward"); break;
    case 'B': autoMode = false; moveBackward(defaultSpeed); Serial.println("Manual: Backward"); break;
    case 'L': autoMode = false; turnLeft(defaultSpeed); Serial.println("Manual: Left"); break;
    case 'R': autoMode = false; turnRight(defaultSpeed); Serial.println("Manual: Right"); break;
    case 'S': autoMode = false; stopBot(); Serial.println("Manual: Stop"); break;
    case 'A': autoMode = true;  Serial.println("Auto mode activated"); break;
    default: break;
  }
}