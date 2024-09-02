#include <Arduino.h>
#include <AccelStepper.h>

// Define pin connections for Motor 1
#define stepPin1 36
#define dirPin1 34 
#define nable1 30

// Define pin connections for Motor 2
#define stepPin2 28  // check the pin num
#define dirPin2 26  // check the pin num
#define nable2 24  // check the pin num

#define LED1 10
#define LED2 9


int incomingbyte = 0;

// Define the motor drivers using AccelStepper
AccelStepper motor1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper motor2(AccelStepper::DRIVER, stepPin2, dirPin2);


void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(stepPin,OUTPUT);  // check out if is neccessary
  pinMode(dirPin,OUTPUT);  // check out if is neccessary
  pinMode(nable, OUTPUT);  // check out if is neccessary
  Serial.begin(9600);
  digitalWrite(nable, LOW);  // check out if is neccessary

  // Set the maximum speed and acceleration for each motor
  motor1.setMaxSpeed(1000);      // Set maximum speed (steps per second)
  motor1.setAcceleration(500);   // Set acceleration (steps per second^2)
  motor1.setCurrentPosition(0);  // Set the current position to 0

  motor2.setMaxSpeed(1000);
  motor2.setAcceleration(500);
  motor2.setCurrentPosition(0);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingbyte = Serial.read();    
    Serial.write(incomingbyte);

  switch (incomingbyte)
  {
  
  // case 48://0
  // //turn motors OFF
  //   digitalWrite(nable, HIGH);
  //   break;

  case 49://1
  //turn LED1 ON
    digitalWrite(LED1, HIGH); 
    break;
  
  case 50://2
  //turn LED1 OFF
    digitalWrite(LED1, LOW); 
    break;
  
  case 51://3
  //turn LED2 ON
    digitalWrite(LED2, HIGH); 
    break;
  
  case 52://4
  //turn LED2 Off
    digitalWrite(LED2, LOW); 
    break;

  case 53://5 rotate 180 deg
  // Move Motor 1 forward by 100 steps
    motor1.moveTo(100);
    motor1.runToPosition();
    break;

  case 54://6 rotate 120 deg
  // Move Motor 1 backward by 67 steps
    motor1.moveTo(67);
    motor1.runToPosition();
    break;

  case 55://7 rotate 180 deg
  // Move Motor 2 forward by 100 steps
    motor2.moveTo(100);
    motor2.runToPosition();
    break;

  case 56://8
  // Move Motor 2 backward by 67 steps
    motor2.moveTo(67);
    motor2.runToPosition();
    break;

  default:
    break;
  

  }
  }


}