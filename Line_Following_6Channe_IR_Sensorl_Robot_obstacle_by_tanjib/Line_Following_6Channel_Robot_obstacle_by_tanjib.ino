// Libraries
#include <NewPing.h>
#include <Servo.h>
#include <AFMotor.h>

// Ultrasonic sensor pins
#define TRIGGER_PIN A4  
#define ECHO_PIN A5
#define MAX_DISTANCE 50

// IR sensor pins (4-channel, using A0–A3)
#define ir2 A0
#define ir3 A1
#define ir4 A2
#define ir5 A3

// Motor setup (L298D Motor Shield using AFMotor library)
AF_DCMotor motor1(1, MOTOR12_1KHZ); // M1 terminal
AF_DCMotor motor2(2, MOTOR12_1KHZ); // M2 terminal

Servo servo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

int distance = 0;
int leftDistance;
int rightDistance;
boolean object;

// Threshold for IR sensors (adjust after testing)
int threshold = 800; // 0–1023 range

void setup() {
  Serial.begin(9600);

  // Servo
  servo.attach(10);
  servo.write(90);

  // Motor speed
  motor1.setSpeed(100);  // adjust 0–255
  motor2.setSpeed(100);
}

void loop() {
  // Read IR sensors as analog, convert to digital (1=black, 0=white)
  int d2 = (analogRead(ir2) < threshold) ? 0 : 1;
  int d3 = (analogRead(ir3) < threshold) ? 0 : 1;
  int d4 = (analogRead(ir4) < threshold) ? 0 : 1;
  int d5 = (analogRead(ir5) < threshold) ? 0 : 1;

  // Debug print
  Serial.print("S2:"); Serial.print(d2);
  Serial.print(" S3:"); Serial.print(d3);
  Serial.print(" S4:"); Serial.print(d4);
  Serial.print(" S5:"); Serial.println(d5);

  // First check obstacle
  distance = getDistance();
  if (distance <= 15) {
    objectAvoid();
  } else {
    // Line following logic
    if(d4 == 0 && d3 == 0){

      moveForward(); 
    }
    else if(d4 == 0 && d3 == 1){

        moveLeft();

    }
    else if (d4 == 1 && d3 == 0){
      moveRight();
    }
    else if (d4 == 1 && d3 == 1){
      Stop();
    }
  }

  delay(50);
}

// ---------------- OBSTACLE AVOIDANCE ----------------
void objectAvoid() {
  Stop();
  distance = getDistance();
  if (distance <= 15) {
    Stop();
    lookLeft();
    lookRight();
    delay(100);
    if (rightDistance <= leftDistance) {
      object = true;
      turn();
    } else {
      object = false;
      turn();
    }
    delay(100);
  }
  else {
    moveForward();
  }
}

int getDistance() {
  delay(50);
  int cm = sonar.ping_cm();
  if (cm == 0) cm = 100;
  return cm;
}

int lookLeft () {
  servo.write(150);
  delay(500);
  leftDistance = getDistance();
  delay(100);
  servo.write(90);
  Serial.print("Left:"); Serial.println(leftDistance);
  return leftDistance;
}

int lookRight() {
  servo.write(30);
  delay(500);
  rightDistance = getDistance();
  delay(100);
  servo.write(90);
  Serial.print("Right:"); Serial.println(rightDistance);
  return rightDistance;
}

// ---------------- MOTOR CONTROL ----------------
void Stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void moveForward() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
}

void moveBackward() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
}

void moveRight() {
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
}

void moveLeft() {
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
}

void turn() {
  if (object == false) {
    moveLeft();
    delay(700);
    moveForward();
    delay(800);
    moveRight();
    delay(900);
  }
  else {
    moveRight();
    delay(700);
    moveForward();
    delay(800);
    moveLeft();
    delay(900);
  }
}
