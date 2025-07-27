"""
class to read serial data from arduino and convert it to motor steps and distance
"""

import serial
import time
from visualizer import PolarVisualizer

class Interpreter:

    def __init__(self, baudrate=115200):
        # default serial port, can be overridden
        port = '/dev/ttyUSB0'  # Change this to your Arduino's
        
    
        # initialize serial port
        self.serial_port = serial.Serial(port, baudrate)
        self.serial_port.flushInput()

    # method to normalize angle (given in motor ticks) to degrees. The angles is expected between -90 and 90, where -512 would be -90 degrees and 512 would be 90 degrees. There are 2048 ticks per rotation
    def normalize_angle(self, angle):
        if not isinstance(angle, (int, float)):
            raise TypeError("Angle must be an integer or float")
        if angle < -512 or angle > 512:
            raise ValueError("Angle must be between -512 and 512")
        
        # normalize to degrees
        # -angle bc angles are inverted in arduino code and easier to fix here
        normalized_angle = (-angle / 512) * 90
        return normalized_angle

    # method to read a line from serial of the form "angle, distance" and parse to in array of form [angle, distance]
    def parse_line(self):
        if self.serial_port.in_waiting > 0:
            line = self.serial_port.readline().decode('utf-8').strip()
            try:
                angle, distance = map(float, line.split(','))
                angle = self.normalize_angle(angle)
                print(f"Received data: Angle = {angle}, Distance = {distance}")
                return [angle, distance]
            except ValueError:
                raise ValueError("Invalid data format received from serial port")
        else:
            return None
       
    # close serial when done
    def close(self):
        self.serial_port.close()



time.sleep(5)
Interpreter = Interpreter()
polar_visualizer = PolarVisualizer()
while True:
    data = Interpreter.parse_line()
    if data is not None:
        polar_visualizer.update_point(data[0], data[1])