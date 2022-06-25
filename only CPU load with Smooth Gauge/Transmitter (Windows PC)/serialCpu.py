from socket import timeout
from time import sleep
import psutil
import serial
import struct

arduino = serial.Serial('COM6', 115200, timeout= 0.03)
sleep(5)

while True:

    p = psutil.cpu_percent()
    p = bytes((str(int(p))), 'utf-8')
    
    arduino.write(p)
    sleep(1)