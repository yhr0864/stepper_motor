# Stepper Motor Control Using Arduino

This project enables remote control of stepper motors through PySerial, interfacing with an Arduino board. The primary goal is to control stepper motors to rotate at specific angles with precision.

## About The Project

![Stepper Motors](./images/stepper_motors.png?raw=true)

In this project, we developed a system to control stepper motors by sending commands from a PC to an Arduino board. The Arduino then processes these commands and drives the motors accordingly.

### Key Features:
- **PC to Arduino Communication:** Commands are sent from the PC via PySerial.
- **Arduino Processing:** The Arduino interprets the received commands and converts them into actions using C.
- **Motor Control:** The motor driver adjusts the motors' behavior based on the commands received.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Getting Started

To get started with this project, follow the instructions below.

### Prerequisites

Ensure you have Python installed and the necessary hardware components (Arduino, stepper motors, motor drivers).

### Installation

Follow these steps to set up the project:

1. Clone the repo
   ```sh
   git clone https://github.com/yhr0864/stepper_motor.git
   ```
2. Install required packages
   ```sh
   pip install -r requirement.txt
   ```
3. Run the Arduino_comunication.py
   ```sh
   python Arduino_comunication.py
   ```
4. Type in the command as you need, e.g.
   ```sh
   1 # LED1 on
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>
