#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_VL53L0X.h>

#define HALFSTEP 4          // motor constant
#define OUT_OF_RANGE -1     // when sensor sees nothing

// max distance left/right we want the motor to go till
const int MAX_ANGLE = 90;
int STEPS_TO_MAX_ANGLE = 0;     // calculate in setup

// motor constants
const int STEPS_PER_ROTATION = 2048; 
const int MAX_SPEED = 80;
const int ACCELERATION = 8000;
const int STEPS_PER_SCAN = 4;   // motor steps in between each scan
int current_location = 0;       // current motor angle in the motor's plane (motor ticks)
bool going_left = true;         // direction of the motor

// Motor / sensor objects
AccelStepper axis(HALFSTEP, 9, 11, 10, 12);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Stores the sensor data, calculate length in setup
int* data = nullptr;

// convert ticks to degrees
int degrees_from_ticks(int ticks)
{
    return (ticks * 360) / STEPS_PER_ROTATION;
}

// convert degrees to ticks
int ticks_from_degrees(int degrees)
{
    return (degrees * STEPS_PER_ROTATION) / 360;
}

// calculate number of items that will be stored
int calculate_num_items()
{
    int rotation_divider = 360 / MAX_ANGLE;
    int ticks_one_direction = 2048 / rotation_divider;
    int total_ticks = (2 * ticks_one_direction) + 1;
    return total_ticks;
}

void setup()
{
    // begin serial
    Serial.begin(9600);
    delay(1000);

    // will need many times so more effiecient to calculate once
    STEPS_TO_MAX_ANGLE = ticks_from_degrees(MAX_ANGLE);

    // initialize data array
    int num_items = calculate_num_items();
    data = new int[num_items];

    // start up vl53l0x
    if (!lox.begin()) {
        Serial.println("Failed to find VL53L0X chip");
        while (1);
    }

    // set motor params
    axis.setMaxSpeed(MAX_SPEED);
    axis.setSpeed(MAX_SPEED);
    axis.setAcceleration(ACCELERATION);
}

// runs the motor in the correct direction for one scan
// updates current location and going left if required
// returns true if direction changed, false otherwise
bool move_motor()
{
    bool ret = false; // set true if changing direction

    // determine the next position
    int next_pos = current_location + STEPS_PER_SCAN;
    if(going_left)
    {
        next_pos = current_location - STEPS_PER_SCAN;
    }        

    // determine if need to switch direction
    if(next_pos > STEPS_TO_MAX_ANGLE || next_pos < -STEPS_TO_MAX_ANGLE)
    {
        going_left = !going_left;
        next_pos = current_location + (going_left ? -STEPS_PER_SCAN: STEPS_PER_SCAN);
        ret = true;
    }

    // move to correct direction
    axis.setCurrentPosition(0);
    if(going_left)
    {
        axis.moveTo(-STEPS_PER_SCAN);
    } 
    else 
    {
        axis.moveTo(STEPS_PER_SCAN);
    }

    while(axis.distanceToGo() != 0)
    {
        axis.run();
    }

}
