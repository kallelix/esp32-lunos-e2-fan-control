#include <Arduino.h>
#include <ArduinoJson.h>

const int DEFAULT_RAMP_DELAY_MS = 80; // delay between fade increments
const int DEFAULT_RAMP_STEP = 4;
const int DEFAULT_CYCLE_DELAY_MS = 30000;
const int SUMMER_CYCLE_DELAY_MS = 600000;

const int SCENARIO_DEFAULT = 0;
const int SCENARIO_LOW = 0;
const int SCENARIO_MID = 1;
const int SCENARIO_HIGH = 2;
const int SCENARIO_HIGHEST = 3;
const int SCENARIO_OFF = 4;
const int SCENARIO_SUMMER = 5;
const int SCENARIO_OUT = 6;

typedef struct {
  String name;
  int channel1;
  int channel2;
  int pin1;
  int pin2;
  int scenario;
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
} Prefs;

int switchScenario(String scenario);

void clearPrefs();
void loadPrefs();
String savePrefs(JsonDocument doc);
int switchScenario(String scenario);