#include "main.h"
#include "prefs.h"

// set relativ speed: -100 to 100
void setSpeed(int pair, int relativspeed, int direction1, int direction2)
{
  int dutyCycle1 = int(float(direction1 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  int dutyCycle2 = int(float(direction2 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  
  ledcWrite(prefs.pairs[pair].channel1, dutyCycle1);
  ledcWrite(prefs.pairs[pair].channel2, dutyCycle2);
  PRINT("Relativ: ");
  PRINT(relativspeed);
  PRINT("Max Duty: ");
  PRINT(MAX_DUTY_CYCLE);
  PRINT(" set pin1: ");
  PRINT(prefs.pairs[pair].pin1);
  PRINT(" set duty1: ");
  PRINT(dutyCycle1);
  PRINT(" set pin2: ");
  PRINT(prefs.pairs[pair].pin2);
  PRINT(" duty2: ");
  PRINTLN(dutyCycle2);
}