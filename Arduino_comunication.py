import serial
import time

arduino = serial.Serial(port="COM8", baudrate=9600, timeout=0.1)


def write_read(x):
    arduino.write(bytes(x, "utf-8"))
    time.sleep(0.005)
    data = arduino.readline()
    return data


while True:
    num = input("Enter a number: ")  # Taking input from user
    value = write_read(num)
    print(value)  # printing the value
