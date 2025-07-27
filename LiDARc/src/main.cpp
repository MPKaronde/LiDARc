#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_VL53L0X.h>

#define HALFSTEP 4          // motor constant
#define OUT_OF_RANGE -1     // when sensor sees nothing

// max distance left/right we want the motor to go till
const int max_angle = 90;

// motor constants
const int STEPS_PER_ROTATION = 2048; 
const int MAX_SPEED = 80;
const int ACCELERATION = 8000;
const int STEPS_PER_SCAN = 4; // motor steps in between each scan

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
    int rotation_divider = 360 / max_angle;
    int ticks_one_direction = 2048 / rotation_divider;
    int total_ticks = (2 * ticks_one_direction) + 1;
    return total_ticks;
}

void setup()
{
    // begin serial
    Serial.begin(9600);
    delay(1000);

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


