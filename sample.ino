// you can change the values below base on your calculations

// define speed
int speedValue = 100; // actual speed = speedValue / 255.0 * max speed

// define time constants
#define run_time 5000   // time (in ms) used to go straight, find by experiment
#define turn_time  1000    // time (in ms) used to turn 90 degrees, find by experiment
#define stop_dist 10   // distance to obstacle to stop, in cm 



// ------- Do not touch below this line -------

// pin numbers
const int trigPin = 12;
const int echoPin = 13;


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

// ------- change below --------

// What functions you have now:
// forward(), backward(), stopCar(), turnLeft(), turnRight(), getDistanceCM()

void loop() {

  // move forward
  unsigned long start = millis(); // record time stamp when robot starts moving forward
  // while moving forward
  while (millis() - start < run_time) {
    // (replace here with your code)
  }

  // turn right
  // (replace here with your code)


  // stop car
  stopCar();
  while(true); // arduino has no 'exit', only way to stop is trap in this loop

}



// ------- sample answer --------
void loop() {
  // ---- move forward with continuous checking ----
  unsigned long start = millis(); 
  while (millis() - start < run_time) {
    // detect obstacle
    float d = getDistanceCM();
    if (d > 0 && d < 10) {
      stopCar();
      while(true); // stop forever
    }
    // move forward
    forward();
    delay(50); 
  }
  stopCar();
  delay(200);

  // ---- turn right (no checking during turn) ----
  turnRight();
  delay(turn_time);
  stopCar();
  delay(200);

  // ---- move forward again with continuous checking ----
  start = millis();
  while (millis() - start < run_time) {
    // detect obstacle
    float d = getDistanceCM();
    if (d > 0 && d < 10) {
      stopCar();
      while(true);
    }
    // move forward
    forward();
    delay(50);
  }
  stopCar();

  // end loop
  while(true); // arduino has no 'exit', only way to stop is trap in this loop
}


