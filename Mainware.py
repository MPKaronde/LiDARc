import serial

class Interpreter:
    def __init__(self, port='/dev/ttyUSB0', baudrate=115200):
        self.serial = serial.Serial(port, baudrate, timeout=1)
    
    def parse_data(self):
        line = self.serial.readline().decode('utf-8').strip()
        angle, distance = map(float, line.split(', '))
        print(f"Angle: {angle}, Distance: {distance}")
    
    def close(self):
        self.serial.close()


Interpreter = Interpreter()
while True:
    try:
        Interpreter.parse_data()
    except Exception as e:
        print(f"Error: {e}")
        break