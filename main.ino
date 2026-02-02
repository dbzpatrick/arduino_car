/*
  Robot car (2x TT DC motors + SA8302 + HC-SR04) — Arduino Uno
  Path: Straight (DIST1) -> Turn Right (90 deg) -> Straight (DIST2) -> Stop
  Safety: Stop immediately if obstacle <= 0.10 m (10 cm)

  POWER PLAN:
  - Motors/SA8302 VM powered by 2x16340 in series (~7.4V)
  - Arduino powered by USB
  - IMPORTANT: Common ground between SA8302 GND and Arduino GND

*/

const int AIN1 = 2;
const int AIN2 = 3;
const int BIN1 = 4;
const int BIN2 = 7;
const int PWMA = 5; // PWM
const int PWMB = 6; // PWM

const int TRIG_PIN = 9;
const int ECHO_PIN = 8;

// ---------- Tune these ----------
const float DIST1_M = 0.80;     // first straight distance (m)
const float DIST2_M = 0.50;     // second straight distance (m)

// Time-based distance needs calibration:
// Measure how far the car travels in 2 seconds at your PWM, then compute speed.
const float STRAIGHT_SPEED_MPS = 0.25; // meters/sec (CHANGE after measuring)

// PWM 0..255
const int PWM_L = 170;          // left motor forward speed
const int PWM_R = 170;          // right motor forward speed

// Turn calibration: adjust until it turns ~90° right on your floor
const unsigned long TURN_RIGHT_90_MS = 520;

// Safety distance
const float OBSTACLE_STOP_M = 0.10; // 10 cm

// Sonar reading period
const unsigned long SONAR_PERIOD_MS = 60;

// ---------- State machine ----------
enum State { RUN1, TURN_RIGHT, RUN2, DONE, EMERGENCY_STOP };
State state = RUN1;

unsigned long stateStartMs = 0;
unsigned long lastSonarMs = 0;
bool obstacle = false;

// ---------- Motor helpers ----------
void setMotorA(int pwm, bool forward) { // Left
  pwm = constrain(pwm, 0, 255);
  if (forward) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }
  analogWrite(PWMA, pwm);
}

void setMotorB(int pwm, bool forward) { // Right
  pwm = constrain(pwm, 0, 255);
  if (forward) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  }
  analogWrite(PWMB, pwm);
}

void stopMotors() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  // coast
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void driveForward() {
  setMotorA(PWM_L, true);
  setMotorB(PWM_R, true);
}

void spinRightInPlace() {
  // left forward, right backward
  setMotorA(PWM_L, true);
  setMotorB(PWM_R, false);
}

// ---------- HC-SR04 ----------
float readDistanceM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // timeout 25ms (~4.3m)
  unsigned long dur = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (dur == 0) return 999.0;

  float cm = (dur * 0.0343f) / 2.0f;
  return cm / 100.0f;
}

bool updateObstacle() {
  unsigned long now = millis();
  if (now - lastSonarMs >= SONAR_PERIOD_MS) {
    lastSonarMs = now;
    float d = readDistanceM();
    obstacle = (d <= OBSTACLE_STOP_M);
    Serial.print("d(m)=");
    Serial.println(d, 3);
  }
  return obstacle;
}

// ---------- Setup ----------
void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.begin(9600);

  stopMotors();
  stateStartMs = millis();
}

// ---------- Main loop ----------
void loop() {
  // Safety check always
  if (updateObstacle()) {
    state = EMERGENCY_STOP;
  }

  unsigned long now = millis();

  switch (state) {
    case RUN1: {
      driveForward();
      unsigned long runMs = (unsigned long)((DIST1_M / STRAIGHT_SPEED_MPS) * 1000.0f);

      if (now - stateStartMs >= runMs) {
        stopMotors();
        state = TURN_RIGHT;
        stateStartMs = now;
        delay(120); // small pause helps consistent turns
      }
      break;
    }

    case TURN_RIGHT: {
      spinRightInPlace();
      if (now - stateStartMs >= TURN_RIGHT_90_MS) {
        stopMotors();
        state = RUN2;
        stateStartMs = now;
        delay(120);
      }
      break;
    }

    case RUN2: {
      driveForward();
      unsigned long runMs = (unsigned long)((DIST2_M / STRAIGHT_SPEED_MPS) * 1000.0f);

      if (now - stateStartMs >= runMs) {
        stopMotors();
        state = DONE;
      }
      break;
    }

    case DONE: {
      stopMotors();
      while (true) { /* finished */ }
      break;
    }

    case EMERGENCY_STOP: {
      stopMotors();
      // safest behavior: stop forever
      while (true) { /* obstacle stop */ }
      break;
    }
  }
}
