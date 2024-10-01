#include <Arduino.h>
#include "pinout.h"

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);
// const int MIN_DUTY_CYCLE = (int)(0);
// const int HALF_DUTY_CYCLE = (int)(MAX_DUTY_CYCLE/2);

// a channel pair for two fans
struct pwm_channel_pair
{
  int channel_1;
  int channel_2;
};

// initial simple scenario, separate restroom from livingroom
const struct pwm_channel_pair living_room = {0, 1};
const struct pwm_channel_pair rest_rooms = {2, 3};

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

  pwm_channel_pair pair;

} Scenario;

const int DEFAULT_RAMP_DELAY_MS = 20; // delay between fade increments
const int DEFAULT_CYCLE_DELAY_MS = 75000;

Scenario off = {DEFAULT_RAMP_DELAY_MS, 1, 0, DEFAULT_CYCLE_DELAY_MS};
Scenario highest = {DEFAULT_RAMP_DELAY_MS, 1, 100, DEFAULT_CYCLE_DELAY_MS};
Scenario high = {DEFAULT_RAMP_DELAY_MS, 1, 70, DEFAULT_CYCLE_DELAY_MS};
Scenario low = {DEFAULT_RAMP_DELAY_MS, 1, 30, DEFAULT_CYCLE_DELAY_MS};

TaskHandle_t Task_living_room;
Scenario Scenario_living_room;

void setup()
{

  // Serial.begin(9600);
  Serial.println("Starting...");

  // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits)
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  ledcSetup(living_room.channel_1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(living_room.channel_2, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(rest_rooms.channel_1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(rest_rooms.channel_2, PWM_FREQ, PWM_RESOLUTION);

  // ledcAttachPin(uint8_t pin, uint8_t channel);
  ledcAttachPin(LED1_OUTPUT_PIN, living_room.channel_1);
  ledcAttachPin(LED2_OUTPUT_PIN, living_room.channel_2);
  ledcAttachPin(FAN1_LIVING_ROOM_OUTPUT_PIN, living_room.channel_1);
  ledcAttachPin(FAN2_LIVING_ROOM_OUTPUT_PIN, living_room.channel_2);

  // init living room
  Scenario_living_room = low;
  Scenario_living_room.pair = living_room;

  xTaskCreatePinnedToCore(
      TaskFanCycle,                  /* Task function. */
      "TaskCycleLivingRoom",         /* name of task. */
      10000,                         /* Stack size of task */
      (void *)&Scenario_living_room, /* parameter of the task */
      1,                             /* priority of the task */
      &Task_living_room,             /* Task handle to keep track of created task */
      0);                            /* pin task to core 0 */
  delay(500);
}

void TaskFanCycle(void *parameter)
{

  Scenario *data = (Scenario *)parameter;
  // Serial.print("Task1 running on core ");
  // Serial.println(xPortGetCoreID());
  for (;;)
  {
    for (int fade = 0; fade <= data->power; fade++)
    {
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = data->power; fade >= (-1 * data->power); fade--)
    {
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = (-1 * data->power); fade <= 0; fade++)
    {
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }
  }
}

// set relativ speed: -100 to 100
void setSpeed(pwm_channel_pair pair, int relativspeed = 0)
{
  int dutyCycle = int(float(relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  int dutyCycleReverse = int(float(-1 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  
  ledcWrite(pair.channel_1, dutyCycle);
  ledcWrite(pair.channel_2, dutyCycleReverse);
  Serial.print("Relativ: ");
  Serial.print(relativspeed);
  Serial.print(" duty1: ");
  Serial.print(dutyCycle);
  Serial.print(" duty2: ");
  Serial.println(dutyCycleReverse);
}

void loop()
{

}
