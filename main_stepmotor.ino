/*
  Stepper Robot Car (NEW PLAN)
  Arduino Uno + 2x 28BYJ-48 (5V) + 2x ULN2003 + HC-SR04 + MB102 (5V) + 2S 16340 input

  Behavior:
    1) Go straight for DIST1 meters
    2) Turn right ~90 deg (calibrated by TURN_RIGHT_STEPS_90)
    3) Go straight for DIST2 meters
    4) Stop
    Safety: Stop immediately if obstacle <= 0.10 m (10 cm) detected by HC-SR04

  WIRING (signals):
    Left ULN2003: IN1 D2, IN2 D3, IN3 D4, IN4 D5
    Right ULN2003: IN1 D6, IN2 D7, IN3 D10, IN4 D11
    HC-SR04: TRIG D9, ECHO D8

  POWER:
    2S battery -> MB102 input
    MB102 set to 5V
    MB102 5V -> Arduino 5V, both ULN2003 VCC, HC-SR04 VCC
    All GND common

  Library needed:
    AccelStepper (Arduino IDE -> Library Manager -> "AccelStepper")
*/

#include <AccelStepper.h>

// ---------------- Pins ----------------
// Left ULN2003 pins
const int L_IN1 = 2;
const int L_IN2 = 3;
const int L_IN3 = 4;
const int L_IN4 = 5;

// Right ULN2003 pins
const int R_IN1 = 6;
const int R_IN2 = 7;
const int R_IN3 = 10;
const int R_IN4 = 11;

// HC-SR04 pins
const int TRIG_PIN = 9;
const int ECHO_PIN = 8;

// ---------------- User-tunable parameters ----------------
const float WHEEL_DIAMETER_M = 0.065f; // 65mm wheel diameter
const float DIST1_M = 0.80f;
const float DIST2_M = 0.50f;

// Obstacle threshold
const float OBSTACLE_STOP_M = 0.10f;

// 28BYJ-48 step counts:
// Many tutorials treat 28BYJ-48 output shaft as 4096 steps per revolution in half-step mode.
// If your distance is off, you'll calibrate this value.
const long STEPS_PER_REV = 4096;

// Turning: number of steps to rotate ~90° in-place.
// You MUST tune this for your chassis (wheel spacing, traction, etc).
const long TURN_RIGHT_STEPS_90 = 1200;

// Motion tuning (reduce if MB102 voltage sags / motors stall)
const float MAX_SPEED_STEPS_PER_S = 700.0f;
const float ACCEL_STEPS_PER_S2    = 500.0f;

// Sonar read interval
const unsigned long SONAR_PERIOD_MS = 60;

// ---------------- Stepper instances ----------------
// For ULN2003 + 28BYJ-48, AccelStepper FULL4WIRE.
// Pin order matters; this order works for many ULN2003 boards.
AccelStepper leftMotor (AccelStepper::FULL4WIRE, L_IN1, L_IN3, L_IN2, L_IN4);
AccelStepper rightMotor(AccelStepper::FULL4WIRE, R_IN1, R_IN3, R_IN2, R_IN4);

// ---------------- State machine ----------------
enum State { RUN1, TURN_RIGHT, RUN2, DONE, EMERGENCY_STOP };
State state = RUN1;

unsigned long lastSonarMs = 0;
bool obstacle = false;

// ---------------- Helpers ----------------
long metersToSteps(float meters) {
  const float PI = 3.1415926f;
  float circumference = PI * WHEEL_DIAMETER_M;
  float revs = meters / circumference;
  return (long)(revs * (float)STEPS_PER_REV);
}

float readDistanceM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // 25ms timeout ~4.3m
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (duration == 0) return 999.0f;

  float distanceCm = (duration * 0.0343f) / 2.0f;
  return distanceCm / 100.0f;
}

bool updateObstacle() {
  unsigned long now = millis();
  if (now - lastSonarMs >= SONAR_PERIOD_MS) {
    lastSonarMs = now;
    float d = readDistanceM();
    obstacle = (d <= OBSTACLE_STOP_M);

    // Debug (optional)
    Serial.print("Distance(m): ");
    Serial.println(d, 3);
  }
  return obstacle;
}

void planStraight(float meters) {
  long s = metersToSteps(meters);
  leftMotor.move(s);
  rightMotor.move(s);
}

void planTurnRight90() {
  // Spin in place: left forward, right backward
  leftMotor.move(TURN_RIGHT_STEPS_90);
  rightMotor.move(-TURN_RIGHT_STEPS_90);
}

void stopNowAndHold() {
  // Stop ramping
  leftMotor.stop();
  rightMotor.stop();

  // Run until both are no longer running (decelerate to stop)
  while (leftMotor.isRunning() || rightMotor.isRunning()) {
    leftMotor.run();
    rightMotor.run();
  }
}

// ---------------- Setup & loop ----------------
void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);

  leftMotor.setMaxSpeed(MAX_SPEED_STEPS_PER_S);
  leftMotor.setAcceleration(ACCEL_STEPS_PER_S2);

  rightMotor.setMaxSpeed(MAX_SPEED_STEPS_PER_S);
  rightMotor.setAcceleration(ACCEL_STEPS_PER_S2);

  // Start first leg
  planStraight(DIST1_M);
  state = RUN1;
}

void loop() {
  // Safety check always
  if (updateObstacle()) {
    state = EMERGENCY_STOP;
  }

  switch (state) {
    case RUN1: {
      leftMotor.run();
      rightMotor.run();
      if (!leftMotor.isRunning() && !rightMotor.isRunning()) {
        planTurnRight90();
        state = TURN_RIGHT;
      }
      break;
    }

    case TURN_RIGHT: {
      leftMotor.run();
      rightMotor.run();
      if (!leftMotor.isRunning() && !rightMotor.isRunning()) {
        planStraight(DIST2_M);
        state = RUN2;
      }
      break;
    }

    case RUN2: {
      leftMotor.run();
      rightMotor.run();
      if (!leftMotor.isRunning() && !rightMotor.isRunning()) {
        state = DONE;
      }
      break;
    }

    case DONE: {
      // Finished: do nothing (motors are already stopped)
      break;
    }

    case EMERGENCY_STOP: {
      stopNowAndHold();
      // Safest: stop forever. Change if you want resume behavior.
      while (true) { }
      break;
    }
  }
}
