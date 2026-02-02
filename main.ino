// sample codes for physics experiments

// ===== Robot car: straight -> right turn -> straight -> stop
// + emergency stop if HC-SR04 detects obstacle <= 0.10 m
//
// Assumes a dual H-bridge driver (L298N/L293D/TB6612 style):
// Left motor: IN1/IN2 + ENA(PWM)
// Right motor: IN3/IN4 + ENB(PWM)

const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 7;

const int ENA = 5; // PWM
const int ENB = 6; // PWM

const int TRIG_PIN = 9;
const int ECHO_PIN = 8;

// ----- User parameters (tune these) -----

// Distances to travel (meters)
const float DIST1_M = 0.80;  // first straight distance
const float DIST2_M = 0.50;  // second straight distance

// Approx straight-line speed at the chosen PWM (meters/second)
// You MUST calibrate this: measure how far it moves in 2-3 seconds.
const float STRAIGHT_SPEED_MPS = 0.20;

// Turn timing (milliseconds) for ~90 degrees right
// Tune this: start around 350–700 ms depending on your car.
const unsigned long RIGHT_TURN_90_MS = 500;

// Motor PWM (0-255). Start ~140-200.
const int PWM_STRAIGHT_L = 170;
const int PWM_STRAIGHT_R = 170;

// For turning: usually one wheel forward, the other backward or stopped.
// Here we spin-in-place (one forward, one backward).
const int PWM_TURN_L = 160;
const int PWM_TURN_R = 160;

// Obstacle stop threshold (meters)
const float OBSTACLE_STOP_M = 0.10;

// Ultrasonic reading interval
const unsigned long SONAR_PERIOD_MS = 60;

// ----- Internal state -----
enum State { RUN1, TURN_RIGHT, RUN2, DONE, EMERGENCY_STOP };
State state = RUN1;

unsigned long stateStartMs = 0;
unsigned long lastSonarMs = 0;

bool obstacleDetected = false;

// ===== Motor helpers =====
void setLeftMotor(int pwm, bool forward) {
  pwm = constrain(pwm, 0, 255);
  if (forward) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(ENA, pwm);
}

void setRightMotor(int pwm, bool forward) {
  pwm = constrain(pwm, 0, 255);
  if (forward) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  analogWrite(ENB, pwm);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  // Optional: brake (depends on driver; for L298N, "both HIGH" brakes)
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void driveForward() {
  setLeftMotor(PWM_STRAIGHT_L, true);
  setRightMotor(PWM_STRAIGHT_R, true);
}

void spinRightInPlace() {
  // left forward, right backward => spin right
  setLeftMotor(PWM_TURN_L, true);
  setRightMotor(PWM_TURN_R, false);
}

// ===== HC-SR04 distance (meters) =====
float readDistanceM() {
  // Trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read echo pulse width (timeout 25ms ~ 4.3m)
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 25000UL);

  if (duration == 0) {
    // No echo (out of range or bad read)
    return 999.0;
  }

  // Speed of sound ~343 m/s => 0.0343 cm/us
  // Distance (cm) = duration * 0.0343 / 2
  float distanceCm = (duration * 0.0343f) / 2.0f;
  return distanceCm / 100.0f;
}

// ===== Setup & loop =====
void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);

  stateStartMs = millis();
  stopMotors();
}

bool checkObstacle() {
  unsigned long now = millis();
  if (now - lastSonarMs >= SONAR_PERIOD_MS) {
    lastSonarMs = now;
    float d = readDistanceM();
    // Simple filter: treat very large as "no obstacle"
    obstacleDetected = (d <= OBSTACLE_STOP_M);
    Serial.print("Distance(m): ");
    Serial.println(d, 3);
  }
  return obstacleDetected;
}

void loop() {
  // Emergency stop check (works in all states)
  if (checkObstacle()) {
    state = EMERGENCY_STOP;
  }

  unsigned long now = millis();

  switch (state) {
    case RUN1: {
      driveForward();
      unsigned long runTimeMs = (unsigned long)((DIST1_M / STRAIGHT_SPEED_MPS) * 1000.0f);

      if (now - stateStartMs >= runTimeMs) {
        stopMotors();
        state = TURN_RIGHT;
        stateStartMs = now;
      }
      break;
    }

    case TURN_RIGHT: {
      spinRightInPlace();
      if (now - stateStartMs >= RIGHT_TURN_90_MS) {
        stopMotors();
        state = RUN2;
        stateStartMs = now;
      }
      break;
    }

    case RUN2: {
      driveForward();
      unsigned long runTimeMs = (unsigned long)((DIST2_M / STRAIGHT_SPEED_MPS) * 1000.0f);

      if (now - stateStartMs >= runTimeMs) {
        stopMotors();
        state = DONE;
      }
      break;
    }

    case DONE: {
      stopMotors();
      // Do nothing
      break;
    }

    case EMERGENCY_STOP: {
      stopMotors();
      // Stay stopped until obstacle cleared (or forever, your choice)
      // Option A: stay stopped forever:
      // while(true);

      // Option B: resume from current state when obstacle clears:
      if (!checkObstacle()) {
        // resume: go back to RUN1/RUN2 etc. (here we just go DONE to be safe)
        state = DONE;
      }
      break;
    }
  }
}
