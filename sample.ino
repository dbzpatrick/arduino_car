// define speed
int speedValue = 100; // actual speed = speedValue / 255.0 * max speed

// define time constants
#define run_time 1200   // time used to go straight, find by experiment
#define turn_time  700    // time used to turn 90 degrees, find by experiment
#define stop_dist 15   // distance to obstacle to stop, in cm




// ------- Do not touch below this line -------

// pin numbers
const int trigPin = 8;
const int echoPin = 7;


// define functions
void forward() {
  //left motor
  digitalWrite(5,LOW);
  analogWrite(6,speedValue);
  //right motor
  analogWrite(9,speedValue);
  digitalWrite(10,LOW);
}

void backward() {
  //left motor
  analogWrite(5,speedValue);
  digitalWrite(6,LOW);
  //right motor
  digitalWrite(9,LOW);
  analogWrite(10,speedValue);
}

void stopCar() {
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
}

void turnLeft() {
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  analogWrite(9,speedValue);
  digitalWrite(10,LOW);
}

void turnRight() {
  digitalWrite(5,LOW);
  analogWrite(6,speedValue);
  digitalWrite(9,LOW);
  digitalWrite(10,LOW);
}

float getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}


// setup pins
void setup(){
  // motors
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  // ultrasonic sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


// ------- Do not touch above this line -------


// main loop to run the car
void loop(){
  // move forward
  long startTime = millis();
  while (millis() - startTime < run_time) {
    float d = getDistanceCM();
    if (d > 0 && d < stop_dist) {
      stopCar();
      while(true);   // stop forever if obstacle detected
    }
    forward();
  }
  stopCar();
  delay(500);

  // turn right
  turnRight();
  delay(turn_time;
  stopCar();
  delay(500);

  // move forward
  startTime = millis();
  while (millis() - startTime < run_time) {
    float d = getDistanceCM();
    if (d > 0 && d < stop_dist) {
      stopCar();
      while(true);
    }
    forward();
  }

  stopCar();
  while(true);
}

