#include <UcTTDcMotor.h>

//define servo motor
#define servoPin A1 //the pin number
#define pulseCycle 20000 // Microseconds
#define shortPulse 900 // Microseconds that corresponds to 0 deg
#define mediumPulse 1550// Microseconds that coresonds to 90 deg
#define longPulse 2100//Microseconds that corresponds to 180 deg
int pulse; //Length of pulse in microseconds

// Define pins for motor control
const int forwardPin1 = D3;  // PWM pin for the first motor's forward motion
const int reversePin1 = D9;  // PWM pin for the first motor's reverse motion
const int forwardPin2 = D10; // PWM pin for the second motor's forward motion
const int reversePin2 = D11; // PWM pin for the second motor's reverse motion

// Create motor instances
UcTTDcMotor motor1(forwardPin1, reversePin1);
UcTTDcMotor motor2(forwardPin2, reversePin2);

// Ultrasonic sensor setup
const int trigPin = 4;    // Trigger pin
const int echoPin = 2;    // Echo pin
long duration;
int distance;

// LED pins
const int greenLedPin1 = 5;
const int greenLedPin2 = 6;
const int yellowLedPin1 = 7;
const int yellowLedPin2 = 8;

bool forwardLedsOn = false;
bool reverseLedsOn = false;

bool isObstacleDetected() {
  distance = getDistance();
  return distance <= 10; // Adjust the threshold as needed
}

void setup() {
  // Initialize both motors
  motor1.init();
  motor2.init();

  // Ultrasonic sensor setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  //Set Up servo
  pinMode(servoPin, OUTPUT);
  turnMiddle();
  delay(1000); // wait a second

  // LED setup
  pinMode(greenLedPin1, OUTPUT);
  pinMode(greenLedPin2, OUTPUT);
  pinMode(yellowLedPin1, OUTPUT);
  pinMode(yellowLedPin2, OUTPUT);

  // Initially, turn on the green LEDs
  turnOnGreenLeds();
}

void loop() {
  // Check if no obstacle is within 10cm
  if (!isObstacleDetected()) {
    // Move both motors forward
    motor1.forward(50);
    motor2.forward(50);

    // Check if forward LEDs are already on, and if not, turn them on
    if (!forwardLedsOn) {
      turnOnGreenLeds();
      forwardLedsOn = true;
      reverseLedsOn = false;
    }
  } else {
    // Stop both motors for 1 second
    motor1.stop();
    motor2.stop();
    delay(500);

    // Move backward 5cm
    moveBackward(2);
    turnLeft();
    delay(500);
    int distanceLeft = getDistance();
    delay(300);
    turnRight();
    delay(500);
    int distanceRight = getDistance();
    delay(300);

    Serial.print("Distance Left: ");
    Serial.println(distanceLeft); // Print right distance for debugging
    Serial.print("Distance Right: ");
    Serial.println(distanceRight); // Print left distance for debugging

    delay(1000);
    turnMiddle();
    if (distanceRight > distanceLeft) {
      turnClockwise(85);
    } else {
      turnAnticlockwise(85);
    }
    // Delay for a moment before resuming forward movement
    delay(100);
    // Move both motors forward
    motor1.forward(50);
    motor2.forward(50);
  }
}

// Function to measure distance with the ultrasonic sensor
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  // Calculate distance in centimeters
  return (duration / 2) / 29.1;
}

// Function to move backward for a specified distance in centimeters
void moveBackward(int cm) {
  motor1.reverse(50);  // Reverse first motor with 50% duty cycle
  motor2.reverse(50);  // Reverse second motor with 50% duty cycle
  int durationMs = cm * 100; // Convert cm to milliseconds
  delay(durationMs);
  motor1.stop();
  motor2.stop();

  // Turn on yellow LEDs when moving backward
  turnOnYellowLeds();
  forwardLedsOn = false;
  reverseLedsOn = true;
}

// Function to turn the robot clockwise by a specified number of degrees
void turnAnticlockwise(int degrees) {
  motor1.forward(50);  // Forward first motor with 50% duty cycle
  motor2.reverse(50);  // Reverse second motor with 50% duty cycle
  int turnDuration = (degrees / 180.0) * 4050; // Calculate turn duration based on desired degrees
  delay(turnDuration);  // Turn for the specified duration
  motor1.stop();
  motor2.stop();

  // Blink yellow LEDs while turning
  blinkLeftYellowLeds();
}

// Function to turn the robot anticlockwise by a specified number of degrees
void turnClockwise(int degrees) {
  motor1.reverse(50);  // Reverse first motor with 50% duty cycle
  motor2.forward(50);  // Forward second motor with 50% duty cycle
  int turnDuration = (degrees / 180.0) * 4050; // Calculate turn duration based on desired degrees
  delay(turnDuration);  // Turn for the specified duration
  motor1.stop();
  motor2.stop();

  // Blink yellow LEDs while turning
  blinkRightYellowLeds();
}

// Function to turn on green LEDs and turn off yellow LEDs
void turnOnGreenLeds() {
  digitalWrite(greenLedPin1, HIGH);
  digitalWrite(greenLedPin2, HIGH);
  digitalWrite(yellowLedPin1, LOW);
  digitalWrite(yellowLedPin2, LOW);
}

// Function to turn on yellow LEDs and turn off green LEDs
void turnOnYellowLeds() {
  digitalWrite(greenLedPin1, LOW);
  digitalWrite(greenLedPin2, LOW);
  digitalWrite(yellowLedPin1, HIGH);
  digitalWrite(yellowLedPin2, HIGH);
}

// Function to blink Left yellow LEDs
void blinkLeftYellowLeds() {
  for (int i = 0; i < 5; i++) {  // Blink 5 times
    digitalWrite(yellowLedPin1, LOW);
    digitalWrite(yellowLedPin2, HIGH);
    delay(500); // 500ms on
    digitalWrite(yellowLedPin1, LOW);
    digitalWrite(yellowLedPin2, LOW);
    delay(500); // 500ms off
  }
}

// Function to blink Right yellow LEDs
void blinkRightYellowLeds() {
  for (int i = 0; i < 5; i++) {  // Blink 5 times
    digitalWrite(yellowLedPin1, HIGH);
    digitalWrite(yellowLedPin2, LOW);
    delay(500); // 500ms on
    digitalWrite(yellowLedPin1, LOW);
    digitalWrite(yellowLedPin2, LOW);
    delay(500); // 500ms off
  }
}

void turnMiddle() {
  // send a pulse of 1850 Microseconds (140 deg)
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(mediumPulse);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(pulseCycle - mediumPulse);
}

void turnLeft() {
  // Send a pulse of 2100 microseconds (180 deg)
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(longPulse);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(pulseCycle - longPulse);
}

void turnRight() {
  // Send a pulse of 1500 microseconds (90 deg)
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(shortPulse);
  digitalWrite(servoPin, LOW);
  delayMicroseconds(pulseCycle - shortPulse);
}