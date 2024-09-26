#include <Arduino.h>
#include <AccelStepper.h>

#define stepPin 26
#define dirPin 28
#define nable 24

#define LED1 10
#define LED2 9
#define SIGNAL 3  // signal from sensor

int incomingbyte = 0;
const char* rotation_finished = "Rotation Finished";
const char* homing_completed = "Homing Completed";

// Define the motor driver using AccelStepper
AccelStepper motor(AccelStepper::DRIVER, stepPin, dirPin);

void moveUntilSensorState(bool targetState, int steps) {
    motor.move(steps); 
    while (digitalRead(SIGNAL) == targetState) {
        motor.run();  // Non-blocking motor run 
    }
    motor.stop();  // Stop the motor 
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(nable, OUTPUT);
  pinMode(SIGNAL, INPUT);
  
  Serial.begin(9600);
  digitalWrite(nable, LOW);

  motor.setMaxSpeed(200);
  motor.setAcceleration(100);
  motor.setCurrentPosition(0);
}

void loop() {
  if (Serial.available() > 0) {
    incomingbyte = Serial.read();    
    // Serial.write(incomingbyte);  // Echo the incoming byte

    switch (incomingbyte) {
      case 49:  // '1' -> Turn LED1 ON
        digitalWrite(LED1, HIGH);
        break;
      
      case 50:  // '2' -> Turn LED1 OFF
        digitalWrite(LED1, LOW);
        break;
      
      case 51:  // '3' -> Turn LED2 ON
        digitalWrite(LED2, HIGH);
        break;
      
      case 52:  // '4' -> Turn LED2 OFF
        digitalWrite(LED2, LOW);
        break;

      case 53:  // '5' -> Rotate 180 degrees
        motor.move(800);  
        motor.runToPosition();
        Serial.println(rotation_finished);
        break;

      case 54:  // '6' -> Rotate 120 degrees
        motor.move(533);
        motor.runToPosition();
        Serial.println(rotation_finished);
        break;

      case 55:  // '7' -> Rotate -180 degrees
        motor.move(-800);
        motor.runToPosition();
        Serial.println(rotation_finished);
        break;

      case 97:  // 'a' -> Home motor
        if (digitalRead(SIGNAL)) {
          moveUntilSensorState(true, 10000);   // Move forward until sensor is not triggered
        }
        moveUntilSensorState(false, -10000);  // Move backward until sensor is triggered

        motor.setCurrentPosition(0);  // Set home position
        Serial.println(homing_completed);
        break;

      default:
        break;
    }
  }
}
