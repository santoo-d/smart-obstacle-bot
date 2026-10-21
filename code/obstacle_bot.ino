/*
  Smart Path Finding Obstacle Detection Bot
  Board  : Arduino Uno + L293D Motor Driver Shield (AFMotor library)
  Sensor : HC-SR04 ultrasonic sensor mounted on a servo

  Behaviour:
  - Drives forward while the path is clear.
  - If an obstacle is closer than STOP_DISTANCE, it stops, reverses a little,
    scans left and right with the servo, and turns toward the clearer side.
*/

#include <AFMotor.h>
#include <Servo.h>

// ---------- Motors (shield ports M1-M4) ----------
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// ---------- Ultrasonic sensor pins (change to match your wiring) ----------
const int TRIG_PIN = A0;
const int ECHO_PIN = A1;

// ---------- Servo ----------
Servo scanServo;
const int SERVO_PIN = 10;      // SERVO_1 header on the shield

// ---------- Settings ----------
const int SPEED = 170;         // 0-255
const int STOP_DISTANCE = 25;  // cm
const int TURN_TIME = 450;     // ms, tune for a ~90 degree turn

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  scanServo.attach(SERVO_PIN);
  scanServo.write(90);         // look straight ahead
  setSpeed(SPEED);
  delay(1000);
}

void loop() {
  int distance = readDistance();

  if (distance == 0 || distance > STOP_DISTANCE) {
    forward();
  } else {
    stopMotors();
    delay(200);
    backward();
    delay(300);
    stopMotors();

    // Scan both sides and pick the clearer path
    scanServo.write(160);
    delay(500);
    int left = readDistance();
    scanServo.write(20);
    delay(500);
    int right = readDistance();
    scanServo.write(90);
    delay(300);

    if (left >= right) turnLeft();
    else turnRight();
    delay(TURN_TIME);
    stopMotors();
  }
  delay(50);
}

// Returns distance in cm (0 = no echo received)
int readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * 0.034 / 2;
}

void setSpeed(int s) {
  motor1.setSpeed(s); motor2.setSpeed(s);
  motor3.setSpeed(s); motor4.setSpeed(s);
}

void forward()    { motor1.run(FORWARD);  motor2.run(FORWARD);  motor3.run(FORWARD);  motor4.run(FORWARD); }
void backward()   { motor1.run(BACKWARD); motor2.run(BACKWARD); motor3.run(BACKWARD); motor4.run(BACKWARD); }
void turnLeft()   { motor1.run(BACKWARD); motor2.run(BACKWARD); motor3.run(FORWARD);  motor4.run(FORWARD); }
void turnRight()  { motor1.run(FORWARD);  motor2.run(FORWARD);  motor3.run(BACKWARD); motor4.run(BACKWARD); }
void stopMotors() { motor1.run(RELEASE);  motor2.run(RELEASE);  motor3.run(RELEASE);  motor4.run(RELEASE); }
