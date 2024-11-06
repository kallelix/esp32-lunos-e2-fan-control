#include <Arduino.h>
#include "prefs.h"
#ifdef DEBUG
#define PRINT(x) Serial.print(x)
#else
#define PRINT(x)
#endif

#ifdef DEBUG
#define PRINTLN(x) Serial.println(x)
#else
#define PRINTLN(x)
#endif

// Papst Product Data Sheet 4412 FGMPR: https://www.farnell.com/datasheets/2634798.pdf
// Lüfter mit Drehrichtungsumkehr. 0% - 45% PWM über Stege blasend, 55% - 100% über Stege saugend.
const int PWM_FREQ = 2000;     // Official ESP32 example uses 5,000Hz as default
const int PWM_RESOLUTION = 8; // resolution: 8 bits, 0-255
// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

// a scenario for an channel pair
typedef struct
{
  // delay between ramp up or down steps
  int ramp_delay_ms;
  // step size
  int ramp_step;
  // power level 0-100
  int power;
  // cycle time
  int cycle_time_ms;
  // direction modifier fan1
  int direction1;
  // direction modifier fan2
  int direction2;

  int pair;

} Scenario;

Scenario off = {DEFAULT_RAMP_DELAY_MS, 4, 0, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario highest = {DEFAULT_RAMP_DELAY_MS, 4, 100, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario high = {DEFAULT_RAMP_DELAY_MS, 4, 70, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario low = {DEFAULT_RAMP_DELAY_MS, 4, 30, DEFAULT_CYCLE_DELAY_MS, 1, -1};
