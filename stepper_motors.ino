#include <Arduino.h>
#include <AccelStepper.h>

// Define motor pins for Motor 1
#define stepPin1 26
#define dirPin1 28
#define nable1 24

// Define motor pins for Motor 2
#define stepPin2 46
#define dirPin2 48
#define nable2 A8

#define LED1 10
#define LED2 9

#define SIGNAL1 3  // signal from sensor
#define SIGNAL2 19  // signal from sensor

#define microSteps 8
#define numBottlesTable1 2  // number of bottles on table -> 180 degrees per rotation
#define numBottlesTable2 6  // number of bottles on table -> 60 degrees per rotation

const char* rotation_finished = "Rotation Finished";
const char* homing_completed = "Homing Completed";

// Command structure
struct MotorCommand {
    bool active;
    bool isHoming;
    int numRotations;  // for last rotation each round we need steps_per_rotation_correction
    int homingPhase;  // 0: not homing, 1: move away, 2: find home
};

MotorCommand motor1Command = {false, false, 1, 0};
MotorCommand motor2Command = {false, false, 1, 0};

// Define the motor drivers
AccelStepper motor1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper motor2(AccelStepper::DRIVER, stepPin2, dirPin2);

// Regular motor movement function
void moveMotor(AccelStepper &motor, MotorCommand &cmd, int numBottles) {
    // for the first (numBottles-1) rotations
    int steps_per_rotation = 200*microSteps/numBottles;
    // for the last rotation: compensate the deviation
    int steps_per_rotation_correction = 200*microSteps-steps_per_rotation*(numBottles-1); 

    if (cmd.numRotations == numBottles) {
        motor.move(steps_per_rotation_correction);
        cmd.active = true;
        cmd.isHoming = false;
        cmd.numRotations = 1;
    } else {
        motor.move(steps_per_rotation);
        cmd.active = true;
        cmd.isHoming = false;
        cmd.numRotations += 1;
    }
    
}

// Homing function
void homeMotor(AccelStepper &motor, MotorCommand &cmd, int SIGNAL) {
    if (!cmd.active) {
        // Start homing sequence
        cmd.active = true;
        cmd.isHoming = true;
        
        if (digitalRead(SIGNAL)) {
            // If sensor triggered, first move away
            cmd.homingPhase = 1;
            motor.move(10000);
        } else {
            // If sensor not triggered, go straight to finding home
            cmd.homingPhase = 2;
            motor.move(-10000);
        }
    } else {
        // Continue homing sequence
        if (cmd.homingPhase == 1 && !digitalRead(SIGNAL)) {
            // Moved away from sensor, now find home
            motor.move(-10000);
            cmd.homingPhase = 2;
        } 
        else if (cmd.homingPhase == 2 && digitalRead(SIGNAL)) {
            // Found home
            motor.stop();
            motor.setCurrentPosition(0);
            cmd.active = false;
            cmd.isHoming = false;
            cmd.numRotations = 1;
            cmd.homingPhase = 0;
            Serial.println(homing_completed);
        }
    }
}

void setup() {
    // Setup pins
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(stepPin1, OUTPUT);
    pinMode(stepPin2, OUTPUT);
    pinMode(dirPin1, OUTPUT);
    pinMode(dirPin2, OUTPUT);
    pinMode(nable1, OUTPUT);
    pinMode(nable2, OUTPUT);
    pinMode(SIGNAL1, INPUT);
    pinMode(SIGNAL2, INPUT);
    
    Serial.begin(9600);
    digitalWrite(nable1, LOW);  
    digitalWrite(nable2, LOW); 

    // Configure motors
    motor1.setMaxSpeed(200);
    motor1.setAcceleration(100);
    motor1.setCurrentPosition(0);

    motor2.setMaxSpeed(200);
    motor2.setAcceleration(100);
    motor2.setCurrentPosition(0);
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        // Handle commands
        switch (cmd) {
            // LED controls
            case '1': digitalWrite(LED1, HIGH); break;
            case '2': digitalWrite(LED1, LOW); break;
            case '3': digitalWrite(LED2, HIGH); break;
            case '4': digitalWrite(LED2, LOW); break;
            
            // Motor 1 controls
            case '5': moveMotor(motor1, motor1Command, numBottlesTable1); break;  // 180 degrees
            case '6': moveMotor(motor1, motor1Command, -numBottlesTable1); break;  // -180 degrees
            case 'a': homeMotor(motor1, motor1Command, SIGNAL1); break;
            
            // Motor 2 controls
            case '7': moveMotor(motor2, motor2Command, numBottlesTable2); break;  // 60 degrees
            case '8': moveMotor(motor2, motor2Command, -numBottlesTable2); break;  // -60 degrees
            case 'b': homeMotor(motor2, motor2Command, SIGNAL2); break;
        }
    }

    // Run active motors
    if (motor1Command.active) {
        motor1.run();
        if (motor1Command.isHoming) {
            homeMotor(motor1, motor1Command, SIGNAL1);  // Homing is not finished, still need to run 
        } else if (motor1.distanceToGo() == 0) {
            motor1Command.active = false;
            Serial.println(rotation_finished);
        }
    }

    if (motor2Command.active) {
        motor2.run();
        if (motor2Command.isHoming) {
            homeMotor(motor2, motor2Command, SIGNAL2);
        } else if (motor2.distanceToGo() == 0) {
            motor2Command.active = false;
            Serial.println(rotation_finished);
        }
    }
}
