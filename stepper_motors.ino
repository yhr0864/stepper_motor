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

const char* rotation_finished = "Rotation Finished";
const char* homing_completed = "Homing Completed";

// Command queue structure
struct MotorCommand {
    char command;
    bool active;
    bool isHoming;
    int homingPhase;  // 0: not homing, 1: moving away from sensor, 2: finding home
};

// Separate command queues for each motor
MotorCommand motor1Command = {0, false, false, 0};
MotorCommand motor2Command = {0, false, false, 0};

// Define the motor driver using AccelStepper
AccelStepper motor1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper motor2(AccelStepper::DRIVER, stepPin2, dirPin2);

void startHoming(AccelStepper &motor, MotorCommand &cmd, int SIGNAL) {
    cmd.isHoming = true;
    cmd.active = true;
    
    if (digitalRead(SIGNAL)) {
        // If sensor is triggered, first move away from sensor
        cmd.homingPhase = 1;
        motor.move(10000);  // Move forward
    } else {
        // If sensor is not triggered, go straight to finding home
        cmd.homingPhase = 2;
        motor.move(-10000);  // Move backward
    }
}

void executeMotor1Command(char command) {
    switch (command) {
        case '5':  // Motor1 rotates 180 degrees
            motor1.move(800);
            motor1Command.active = true;
            motor1Command.isHoming = false;
            break;
        case '6':  // Motor1 rotates 120 degrees
            motor1.move(533);
            motor1Command.active = true;
            motor1Command.isHoming = false;
            break;
        case 'a':  // Home motor1
            startHoming(motor1, motor1Command, SIGNAL1);
            break;
    }
}

void executeMotor2Command(char command) {
    switch (command) {
        case '7':  // Motor2 rotates 180 degrees
            motor2.move(800);
            motor2Command.active = true;
            motor2Command.isHoming = false;
            break;
        case '8':  // Motor2 rotates 120 degrees
            motor2.move(533);
            motor2Command.active = true;
            motor2Command.isHoming = false;
            break;
        case 'b':  // Home motor2
            startHoming(motor2, motor2Command, SIGNAL2);
            break;
    }
}

void processHoming(AccelStepper &motor, MotorCommand &cmd, int SIGNAL) {
    if (!cmd.isHoming) return;

    if (cmd.homingPhase == 1) {
        // Moving away from sensor
        if (motor.distanceToGo() == 0 || !digitalRead(SIGNAL)) {
            // We've moved away from sensor, now find home
            cmd.homingPhase = 2;
            motor.move(-10000);  // Move backward to find home
        }
    }
    else if (cmd.homingPhase == 2) {
        // Finding home position
        if (digitalRead(SIGNAL)) {
            // Found home
            motor.stop();
            motor.setCurrentPosition(0);
            cmd.active = false;
            cmd.isHoming = false;
            cmd.homingPhase = 0;
            Serial.println(homing_completed);
        }
    }
}

void setup() {
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

    motor1.setMaxSpeed(200);
    motor1.setAcceleration(100);
    motor1.setCurrentPosition(0);

    motor2.setMaxSpeed(200);
    motor2.setAcceleration(100);
    motor2.setCurrentPosition(0);
}

void loop() {
    // Check for new commands
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();

        // Handle LED commands immediately
        switch (incomingByte) {
            case '1': digitalWrite(LED1, HIGH); break;
            case '2': digitalWrite(LED1, LOW); break;
            case '3': digitalWrite(LED2, HIGH); break;
            case '4': digitalWrite(LED2, LOW); break;
            
            // Special case for simultaneous 180-degree rotation
            case '9':
                motor1.move(800);
                motor2.move(800);
                motor1Command.active = true;
                motor2Command.active = true;
                motor1Command.isHoming = false;
                motor2Command.isHoming = false;
                break;
                
            default:
                // Queue motor commands based on which motor they're for
                if (incomingByte == '5' || incomingByte == '6' || incomingByte == 'a') {
                    motor1Command.command = incomingByte;
                    executeMotor1Command(incomingByte);
                }
                else if (incomingByte == '7' || incomingByte == '8' || incomingByte == 'b') {
                    motor2Command.command = incomingByte;
                    executeMotor2Command(incomingByte);
                }
                break;
        }
    }

    // Run both motors if they have active commands
    if (motor1Command.active) {
        motor1.run();
        if (motor1Command.isHoming) {
            processHoming(motor1, motor1Command, SIGNAL1);
        } else if (motor1.distanceToGo() == 0) {
            motor1Command.active = false;
            Serial.println(rotation_finished);
        }
    }

    if (motor2Command.active) {
        motor2.run();
        if (motor2Command.isHoming) {
            processHoming(motor2, motor2Command, SIGNAL2);
        } else if (motor2.distanceToGo() == 0) {
            motor2Command.active = false;
            Serial.println(rotation_finished);
        }
    }
}
