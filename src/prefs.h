#include <Arduino.h>
#include <ArduinoJson.h>

const int DEFAULT_RAMP_DELAY_MS = 40; // delay between fade increments
const int DEFAULT_RAMP_STEP = 4;
const int DEFAULT_CYCLE_DELAY_MS = 30000;

typedef struct {
  int channel1;
  int channel2;
  int pin1;
  int pin2;
} Pair;

typedef struct
{
  String hostname;

  int no_pairs;

  Pair pairs[5];
  // delay between ramp up or down steps
  int ramp_delay_ms;
  // step size
  int ramp_step;
  // cycle time
  int cycle_time_ms;
  // power level 0-100
  int power;
  // direction modifier fan1
  int direction1;
  // direction modifier fan2
  int direction2;

} Prefs;

Prefs prefs = {"lunos-fan-control", 1, {{0,1,3,4},{2,3,0,0},{4,5,0,0},{6,7,0,0},{8,9,0,0}}, DEFAULT_RAMP_DELAY_MS, DEFAULT_RAMP_STEP, DEFAULT_CYCLE_DELAY_MS, 30, 1, -1};

void loadPrefs();
void clearPrefs();
void savePrefs(JsonDocument doc);
