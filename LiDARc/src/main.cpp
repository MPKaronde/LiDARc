#include <Arduino.h>
#include <AccelStepper.h>
#include <Adafruit_VL53L0X.h>

/*
2048 steps / revolution
therefore, 90 deeegrees = 2048 / 4 = 512 steps
*/

// max steps on left to sweep
int max_left_angle = -512;

// max steps on right to sweep
int max_right_angle = 512;

// constants for stepper motor
#define HALFSTEP 4
// constant for reading out of range (makes it -1)
#define OUT_OF_RANGE 2147483647

// motor and sensor objects
AccelStepper axis(HALFSTEP, 9, 11, 10, 12);
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// num steps motor moved in between readings
// for precision, must be factor of max angles
int sweep_angle = 4;

// current position of the motor
int current_position = 0;

// are we running left or right?
// NOTE: left is negative, right is positive
bool moving_left = true;


void setup() {
  // serial setup
  Serial.begin(115200);

  delay(1000); // give time for serial to initialize

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
  axis.setMaxSpeed(80);
  axis.setAcceleration(8000);
  axis.setSpeed(80);
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

  while(axis.distanceToGo() != 0) {
    axis.run();
  }

  current_position = nextPos;
}

void read_and_proceed()
{
  // read distance from sensor
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  int distance = measure.RangeMilliMeter;
  if (measure.RangeStatus == 4) { // make sure reading in range
    distance = OUT_OF_RANGE;
  }

  // send reading to serial
  // output in format: "current_position, distance"
  Serial.print(current_position);
  Serial.print(", ");
  Serial.println(distance);

  // move motor to next position
  move_motor();
}

bool start = true;

void loop()
{
  if(start)
  {
    delay(1000);
    start = false;
  }
  read_and_proceed();
} 


  