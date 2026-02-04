// define speed
int speedValue = 100; // actual speed = speedValue / 255.0 * max speed

// define time constants
#define MOVE_50CM_TIME 1200   // time used to move 50cm, find by experiment
#define TURN_90_TIME   700    // time used to turn 90 degrees, find by experiment


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

// setup pins
void setup(){
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

// main loop to run the car
void loop(){

  // 1. Go forward 50 cm
  forward();
  delay(MOVE_50CM_TIME);
  stopCar();
  delay(500);

  // 2. Turn right 90 degrees
  turnRight();
  delay(TURN_90_TIME);
  stopCar();
  delay(500);

  // 3. Go forward another 50 cm
  forward();
  delay(MOVE_50CM_TIME);
  stopCar();

  while(true); // stop forever
}
