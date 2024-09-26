import serial

arduino = serial.Serial(port="COM8", baudrate=9600, timeout=0.1)


def write_read(x):
    # Send data to Arduino
    arduino.write(bytes(x, "utf-8"))  # Send with newline

    # Wait for a response
    while True:
        data = arduino.readline().decode("utf-8").strip()
        if data:  # Check if data is received
            return data


while True:
    num = input("Enter a number: ")  # Taking input from user
    value = write_read(num)  # Send input and get the response
    print("Received:", value)  # Print the response
