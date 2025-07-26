"""
class to store data from the rotating sensor w/ parameters to change resolution based on how sensor is initialized

Distance stored in mm
Angles stored in motor steps internally but w/ methods to translate to degrees externally
"""

class Map:

    # max angle & steps per reading are in motor steps
    def __init__(self, max_angle, steps_per_reading, steps_per_revolution = 2048):
        self.max_angle = max_angle                          # max angle in one direction sensor will travel to
        self.steps_per_reading = steps_per_reading          # number of steps between each reading
        self.size = (2 * max_angle / steps_per_reading) + 1 # numb readings stored
        self.data = [0] * self.size                         # array of all the readings
        self.steps_per_revolution = steps_per_revolution    # number of steps in one full revolution of the motor

    # get angle in degrees from motor steps by converting with steps_per_revolution
    def steps_to_degrees(self, angle):
        if angle < -self.max_angle or angle > self.max_angle or angle % self.steps_per_reading != 0:
            raise ValueError("Angle out of bounds")
        
        # convert motor steps to degrees
        return (angle / self.steps_per_revolution) * 360.0

    # get angle in motor steps from degrees
    def degrees_to_steps(self, angle):
        if angle < -self.max_angle or angle > self.max_angle:
            raise ValueError("Angle out of bounds")
        
        # convert degrees to motor steps
        return int((angle / 360.0) * self.steps_per_revolution)

    # angle in motor steps, distance in mm
    def enter_data(self, angle, distance):
        # make sure angle is within bounds and is a multiple of steps_per_reading
        if angle < -self.max_angle or angle > self.max_angle or angle % self.steps_per_reading != 0:
            raise ValueError("Angle out of bounds")
        
        # get index by factoring out stepper_reading & normalizing to 0
        index = (max_angle / self.steps_per_reading) + (angle / self.steps_per_reading)

        # add data to array
        self.data[int(index)] = distance

    # get distance at a particular index in the array
    def get_data_by_index(self, index):
        if index < 0 or index >= self.size:
            raise IndexError("Index out of bounds")
        return self.data[index]

    # get distance at a particular angle in motor steps
    def get_data_by_angle(self, angle):
        if angle < -self.max_angle or angle > self.max_angle or angle % self.steps_per_reading != 0:
            raise ValueError("Angle out of bounds")
        
        # get index by factoring out stepper_reading & normalizing to 0
        index = (self.max_angle / self.steps_per_reading) + (angle / self.steps_per_reading)
        return self.get_data_by_index(int(index))

    # get distance at a particular angle in degrees
    def get_data_by_degrees(self, degrees):
        if degrees < -self.steps_to_degrees(self.max_angle) or degrees > self.steps_to_degrees(self.max_angle):
            raise ValueError("Angle out of bounds") 
        # convert degrees to motor steps
        angle = self.degrees_to_steps(degrees)

        # round down to nearest step
        if(angle % self.steps_per_reading != 0):
            int error = angle % self.steps_per_reading
            if(angle > 0):
                angle -= error  
            else:
                angle += error
        
        return self.get_data_by_angle(angle)

    


        


    