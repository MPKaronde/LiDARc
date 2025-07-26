#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_VL53L0X.h>

// constants for stepper motor
#define HALFSTEP 4
// constant for reading out of range (at arduinos integer max value)
#define OUT_OF_RANGE 2147483647

// motor and sensor objects
AccelStepper axis(HALFSTEP, 0, 1, 2, 3);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// num steps motor moved in between readings
int sweep_angle = 5;

// max angle on left to sweep
int max_left_angle = -90;

// max angle on right to sweep
int max_right_angle = 90;

// current position of the motor
int current_position = 0;

// are we running left or right?
// NOTE: left is negative, right is positive
bool moving_left = true;


void setup() {
  // serial setup
  Serial.begin(115200);

  // wait for serial to be ready
  while (!Serial) {
    delay(10); 
  }

  // wait for the VL53L0X to be ready
  if (!lox.begin()) {
    Serial.println("Failed to find VL53L0X chip");
    while (1);
  }

  // set motor parameters
  axis.setMaxSpeed(100);
  axis.setAcceleration(80);
}

//moved the motor to the next position, if next position is out of bounds reverse direction
void move_motor()
{
  // determine next position
  int nextPos = current_position + sweep_angle;
  if(moving_left)
  {
    nextPos = current_position - sweep_angle;
  }

  // change directions if needed
  if(nextPos > max_right_angle || nextPos < max_left_angle)
  {
    moving_left = !moving_left;
    nextPos = current_position + (moving_left ? -sweep_angle : sweep_angle);
  }

  // move motor to appropriate position
  axis.setCurrentPosition(0);
  if(moving_left)
  {
    axis.moveTo(-sweep_angle);
  }
  else
  {
    axis.moveTo(sweep_angle);
  }
}

void read_and_proceed()
{
  // read distance from sensor
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  // send reading to serial
  if (measure.RangeStatus != 4) { // make sure reading in range
    Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(OUT_OF_RANGE);
  }

  // move motor to next position
  move_motor();
}

void loop()
{
  move_motor();
}


