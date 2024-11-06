#include "main.h"
#include "../helper.h"

void TaskDefaultFanCycle(void *parameter)
{
  Scenario *data = (Scenario *)parameter;
  PRINT("Task FanCycle running on core ");
  PRINTLN(xPortGetCoreID());
  for (;;)
  {
    for (int fade = 0; fade <= data->power; fade+=data->ramp_step)
    {
      setSpeed(data->pair, fade, data->direction1, data->direction2);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = data->power; fade >= (-1 * data->power); fade-=data->ramp_step)
    {
      setSpeed(data->pair, fade, data->direction1, data->direction2);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = (-1 * data->power); fade <= 0; fade+=data->ramp_step)
    {
      setSpeed(data->pair, fade, data->direction1, data->direction2);
      delay(data->ramp_delay_ms);
    }
  }
}
