#include <Arduino.h>
#include <AccelStepper.h>

// Pin definitions
#define stepPin1 26
#define dirPin1 28
#define nable1 24

#define stepPin2 46
#define dirPin2 48
#define nable2 A8

#define LED1 10
#define LED2 9

#define SIGNAL1 3
#define SIGNAL2 19

// Common constants
#define microSteps 8
#define numBottlesTable1 2
#define numBottlesTable2 6
const char* rotation_finished = "Rotation Finished";
const char* homing_completed = "Homing Completed";

class Motor {
public:
    AccelStepper& stepper;
    const int stepPin;
    const int dirPin;
    const int sensorPin;
    const int enablePin;
    const int numBottles;
    
    // Command state
    bool isActive;
    bool isHoming;
    int numRotations;
    int homingPhase;
    
    // Constructor
    Motor(AccelStepper& _stepper, int _stepPin, int _dirPin, int _sensorPin, int _enablePin, int _numBottles) 
        : stepper(_stepper), 
          stepPin(_stepPin),
          dirPin(_dirPin),
          sensorPin(_sensorPin), 
          enablePin(_enablePin), 
          numBottles(_numBottles),
          isActive(false),
          isHoming(false),
          numRotations(1),
          homingPhase(0) {}

public:
    virtual void init() {
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
   
        pinMode(enablePin, OUTPUT);
        pinMode(sensorPin, INPUT);
        
        digitalWrite(enablePin, LOW);
        
        stepper.setMaxSpeed(200);
        stepper.setAcceleration(100);
    }

    void LED1_on() {
        digitalWrite(LED1, HIGH);
    }

    void LED1_off() {
        digitalWrite(LED1, LOW);
    }

    void LED2_on() {
        digitalWrite(LED2, HIGH);
    }

    void LED2_off() {
        digitalWrite(LED2, LOW);
    }

    void rotate() {
        if (isActive) return;
        
        // Calculate steps for rotation
        int steps_per_rotation = 200 * microSteps / numBottles;
        int steps_per_rotation_correction = 200 * microSteps - steps_per_rotation * (numBottles - 1);
        
        if (numRotations == numBottles) {
            stepper.move(steps_per_rotation_correction);
            numRotations = 1;
        } else {
            stepper.move(steps_per_rotation);
            numRotations++;
        }
        
        isActive = true;
        isHoming = false;
    }

    void home() {
        if (!isActive) {
            // Start homing sequence
            isActive = true;
            isHoming = true;
            
            if (digitalRead(sensorPin)) {
                // If sensor triggered, first move away
                homingPhase = 1;
                stepper.move(10000);
            } else {
                // If sensor not triggered, go straight to finding home
                homingPhase = 2;
                stepper.move(-10000);
            }
        } else {
            // Continue homing sequence
            if (homingPhase == 1 && !digitalRead(sensorPin)) {
                // Moved away from sensor, now find home
                stepper.move(-10000);
                homingPhase = 2;
            } 
            else if (homingPhase == 2 && digitalRead(sensorPin)) {
                // Found home
                stepper.stop();
                stepper.setCurrentPosition(0);
                isActive = false;
                isHoming = false;
                numRotations = 1;
                homingPhase = 0;
                Serial.println(homing_completed);
            }
        }
    }

    void update() {
        if (isActive) {
            stepper.run();
            if (isHoming) {
                home();  // Continue homing sequence
            } else if (stepper.distanceToGo() == 0) {
                isActive = false;
                Serial.println(rotation_finished);
            }
        }
    }
};


// Create motor instances
AccelStepper stepper1(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepper2(AccelStepper::DRIVER, stepPin2, dirPin2);

Motor motor1(stepper1, stepPin1, dirPin1, SIGNAL1, nable1, numBottlesTable1);
Motor motor2(stepper2, stepPin2, dirPin2, SIGNAL2, nable2, numBottlesTable2);

void setup() {
    Serial.begin(9600);
    
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    // Initialize both motors
    motor1.init();
    motor2.init();
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        
        // To parse this part
        switch (cmd) {
            // LED controls
            // case '1': motor1.LED1_on(); break;
            // case '2': motor1.LED1_off(); break;
            // case '3': digitalWrite(LED2, HIGH); break;
            // case '4': digitalWrite(LED2, LOW); break;

            // Motor controls
            case '5': motor1.rotate(); break;
            case '6': motor2.rotate(); break;
            case 'a': motor1.home(); break;
            case 'b': motor2.home(); break;
        }
    }

    // Update motor states
    motor1.update();
    motor2.update();
}
