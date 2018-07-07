import serial
import time


s = serial.Serial('/dev/ttyACM0', 9600)
time.sleep(2)
s.write("OFF\n")
time.sleep(10)
s.write("ON\n")
s.close()
