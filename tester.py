""" some code to test arduino code, Interpreter.py and Map.py with written and digital confirmation"""
import serial
from Map import Map


import matplotlib
matplotlib.use('TkAgg') 
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.pyplot as plt

# Add this once at the top of your script
plt.ion()


class SweepPlot:
    def __init__(self):
        self.steps_per_rotation = 2048
        self.step_range = (-512, 512)
        self.step_interval = 4
        self.degrees_per_step = 360 / self.steps_per_rotation

        self.indices = np.arange((self.step_range[1] - self.step_range[0]) // self.step_interval + 1)
        self.motor_steps = self.step_range[0] + self.indices * self.step_interval

        # Original angles in degrees: from -90 to +90
        self.angles_deg = self.motor_steps * self.degrees_per_step

        # Shift angles from [-90, 90] to [0, 180]
        self.angles_deg_shifted = self.angles_deg + 90  
        self.angles_rad = np.deg2rad(self.angles_deg_shifted)

        self.fig, self.ax = plt.subplots(figsize=(6,6), subplot_kw={'projection': 'polar'})
        self.ax.set_title("Polar Distance Sensor Map (0° to 180° shifted)")
        self.ax.set_theta_zero_location('N')  # 0° at top
        self.ax.set_theta_direction(-1)       # Clockwise
        self.ax.set_thetamin(0)
        self.ax.set_thetamax(180)

        self.line, = self.ax.plot([], [], marker='o', linestyle='-')
        plt.ion()
        plt.show()

    def update(self, distance_data):
        self.line.set_data(self.angles_rad, distance_data)
        self.ax.relim()
        self.ax.autoscale_view()
        self.fig.canvas.draw()
        self.fig.canvas.flush_events()




ser = serial.Serial('/dev/ttyUSB0', 115200)  # adjust to your device

map = Map(512, 4)

import time

plotter = SweepPlot()

while (1 == 1):
    line = ser.readline().decode('utf-8').strip()
    values = [int(x.strip()) for x in line.split(',')]
    print("Parsed values:", values)


    print("values[1] : " + str(values[1]))
    map.enter_data(values[0], values[1])
    map.print_all_data()

    plotter.update(map.get_all_data())

