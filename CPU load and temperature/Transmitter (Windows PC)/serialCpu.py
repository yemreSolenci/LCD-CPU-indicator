from socket import timeout
from time import sleep
from tkinter import N
import psutil
import serial
import sys
import ctypes
import clr

arduinoConnect = True

def send_serial(l, t):
    global arduino
    d = "*" + l + "," + t
    print(d)
    d = convert_bytes(d)
    try:
        arduino.write(d)
    except serial.SerialException as e:
        print("Connection Lost  ", e)
        global arduinoConnect
        arduinoConnect = False


def convert_bytes(v):
    return bytes(v, 'utf-8')

def cpu_load():
    return int(psutil.cpu_percent())

def fetch_temp(c):
    for a in range(0, len(c.Hardware[0].Sensors)):
        c.Hardware[0].Update()
        if "/temperature" in str(c.Hardware[0].Sensors[a].Identifier):
            t = int(c.Hardware[0].Sensors[a].get_Value())
    return t

def initialize_openhardwaremonitor():
    clr.AddReference(r'OpenHardwareMonitorLib')
    from OpenHardwareMonitor.Hardware import Computer

    c = Computer()
    c.CPUEnabled = True
    c.Open()
    return c

def open_serial():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    else:
        ctypes.windll.user32.MessageBoxW(0, "This program support only Windows", "Unsupported Platform", 0)
        raise EnvironmentError('Unsupported platform')
    print("Ports scanning", end= " ", flush=True)
    while True:
        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        print(".", end= "", flush=True)
        sleep(0.5)
        if result != []:
            print()
            sleep(4)
            break
    
    global arduinoConnect
    arduinoConnect = True
    arduino = serial.Serial(result[0], 115200, timeout= 0.03, dsrdtr=None)
    return arduino
     

if __name__ == "__main__":
    arduino = open_serial()
    c = initialize_openhardwaremonitor()
    sleep(2)
    while True:
        if arduinoConnect:
            l = str(cpu_load())
            t = str(fetch_temp(c))
            send_serial(l, t)
        else:
            arduino = open_serial()
        sleep(1)