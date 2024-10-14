// #include <map>
// #include <string>
// #include <functional>
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

// Function pointer type
typedef void (*CommandFunction)();

// Command functions
void led1On() { digitalWrite(LED1, HIGH); }
void led1Off() { digitalWrite(LED1, LOW); }
void led2On() { digitalWrite(LED2, HIGH); }
void led2Off() { digitalWrite(LED2, LOW); }
void motor1Rotate() { motor1.rotate(); }
void motor1Home() { motor1.home(); }
void motor2Rotate() { motor2.rotate(); }
void motor2Home() { motor2.home(); }

// Command structure
struct Command {
    const char* name;
    CommandFunction function;
};

// Command lookup table
const Command commands[] = {
    {"LED1 on", led1On},
    {"LED1 off", led1Off},
    {"LED2 on", led2On},
    {"LED2 off", led2Off},
    {"motor1 rotate", motor1Rotate},
    {"motor1 home", motor1Home},
    {"motor2 rotate", motor2Rotate},
    {"motor2 home", motor2Home},
    {NULL, NULL}  // Sentinel to mark the end of the array
};

void executeCommand(const char* cmd) {
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(cmd, commands[i].name) == 0) {
            commands[i].function();
            return;
        }
    }
    Serial.println("Unknown command");
}

void setup() {
    Serial.begin(9600);
    
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);

    motor1.init();
    motor2.init();
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n'); 

        // Remove any trailing whitespace or non-printable characters
        cmd.trim();

        // // Debug print the received command
        // Serial.print("Received command: '");
        // Serial.print(cmd);
        // Serial.print("'\n");

        executeCommand(cmd.c_str());
    }

    motor1.update();
    motor2.update();
}
