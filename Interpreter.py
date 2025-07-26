"""
class to read serial data from arduino and convert it to motor steps and distance
"""

import serial


class Interpreter:

    def __init__(self, baudrate=115200):
        # code to find the serial port
        ports = serial.tools.list_ports.comports()
        port = None
        for p in ports:
            if 'Arduino' in p.description or 'ttyUSB' in p.device:
                port = p.device
                break   
        if port is None:
            raise ValueError("No Arduino found. Please connect an Arduino and try again.")
    
        # initialize serial port
        self.serial_port = serial.Serial(port, baudrate)
        self.serial_port.flushInput()

    # method to read a line from serial of the form "angle, distance" and parse to in array of form [angle, distance]
    def parse_line(self):
        if self.serial_port.in_waiting > 0:
            line = self.serial_port.readline().decode('utf-8').strip()
            try:
                angle, distance = map(float, line.split(','))
                return [angle, distance]
            except ValueError:
                raise ValueError("Invalid data format received from serial port")
        else:
            return None
       
    # close serial when done
    def close(self):
        self.serial_port.close()