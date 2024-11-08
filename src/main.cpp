#include "main.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "settings.h"

// Web server running on port 80
WebServer server(80);

TaskHandle_t tasks[5];
Scenario pairs[5];

extern Prefs prefs;

void setScenario(int pair, int scenario)
{
  switch (scenario)
  {
  case SCENARIO_LOW:
    pairs[pair].power = 30;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_MID:
    pairs[pair].power = 50;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_HIGH:
    pairs[pair].power = 70;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_HIGHEST:
    pairs[pair].power = 100;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_OFF:
    pairs[pair].power = 0;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_SUMMER_NIGHT:
    pairs[pair].power = 30;
    pairs[pair].cycle_time_ms = SUMMER_CYCLE_DELAY_MS;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = -1;
    // 8 hours
    pairs[pair].ttl = 28800000;
    break;
  case SCENARIO_IN:
    pairs[pair].power = 30;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = -1;
    pairs[pair].direction2 = -1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_OUT:
    pairs[pair].power = 30;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = 1;
    pairs[pair].ttl = 0;
    break;
  case SCENARIO_SHORT_SHOOTOUT:
    pairs[pair].power = 100;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].direction1 = 1;
    pairs[pair].direction2 = 1;
    // 15 minutes
    pairs[pair].ttl = 900000;
    break;
  case SCENARIO_NIGHT:
    pairs[pair].power = 25;
    pairs[pair].cycle_time_ms = prefs.cycle_time_ms;
    pairs[pair].ramp_delay_ms = prefs.ramp_delay_ms*2;
    pairs[pair].direction1 = -1;
    pairs[pair].direction2 = 1;
    // 8 hours
    pairs[pair].ttl = 28800000;
    break;
  default:
    pairs[pair].power = 30;
  }
}

void getInfo()
{
  String json;
  JsonDocument doc;
  PRINTLN("Get Fan Data");
  doc["hostname"] = prefs.hostname;
  doc["no_pairs"] = prefs.no_pairs;
  for (int i = 0; i < prefs.no_pairs; i++)
  {
    JsonObject obj = doc["fanpairs"].add<JsonObject>();
    obj["name"] = prefs.pairs[i].name;
    obj["power"] = pairs[i].power;
    obj["cycletime"] = pairs[i].cycle_time_ms;
  }
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleSet()
{
  if (server.hasArg("plain") == false)
  {
    server.send(400, "application/json", "{}");
  }
  else
  {
    JsonDocument doc;
    String body = server.arg("plain");
    deserializeJson(doc, body);

    String fanpair = doc["fanpair"];
    String action = "nop";
    for (int i = 0; i < prefs.no_pairs; i++)
    {
      if (prefs.pairs[i].name == fanpair)
      {
        action = "modified";
        if (!doc["scenario"].isNull())
        {
          String scenario = doc["scenario"];
          int scenario_int = switchScenario(scenario);
          setScenario(i, scenario_int);
        }
        if (!doc["power"].isNull())
        {
          int power = doc["power"];
          pairs[i].power = power;
        }
        if (!doc["cycletime"].isNull())
        {
          int cycletime = doc["cycletime"];
          pairs[i].cycle_time_ms = cycletime;
        }
      }
    }

    String json;
    JsonDocument rdoc;
    rdoc["fanpair"] = fanpair;
    rdoc["action"] = action;

    serializeJson(rdoc, json);

    server.send(200, "application/json", json);
  }
}

void handleConfig()
{
  if (server.hasArg("plain") == false)
  {
    server.send(400, "application/json", "{}");
  }
  else
  {
    JsonDocument doc;
    String body = server.arg("plain");
    deserializeJson(doc, body);
    String json = savePrefs(doc);
    server.send(200, "application/json", json);
    delay(500);
    ESP.restart();
  }
}

void handleClear()
{
  if (server.hasArg("plain") == false)
  {
    server.send(400, "application/json", "{}");
  }
  else
  {
    JsonDocument doc;
    String body = server.arg("plain");
    deserializeJson(doc, body);
    clearPrefs();
    server.send(200, "application/json", "{}");
  }
}

void setup_routing()
{
  server.on("/info", getInfo);
  server.on("/set", HTTP_POST, handleSet);
  server.on("/config", HTTP_POST, handleConfig);
  server.on("/clear", HTTP_POST, handleClear);

  server.begin();
}

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

void TaskFanCycle(void *parameter)
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

void setup()
{

#ifdef DEBUG
  Serial.begin(9600);
#endif

  loadPrefs();

  PRINTLN("Starting...");
  WiFiManager wm;
  wm.setHostname(prefs.hostname);
  wm.autoConnect("AutoConnectAP", "password");

  // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits)
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  for (int i = 0; i < prefs.no_pairs; i++)
  {
    ledcSetup(prefs.pairs[i].channel1, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(prefs.pairs[i].channel2, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(prefs.pairs[i].pin1, prefs.pairs[i].channel1);
    ledcAttachPin(prefs.pairs[i].pin2, prefs.pairs[i].channel2);
  }

  // init pairs
  for (int i = 0; i < prefs.no_pairs; i++)
  {
    pairs[i].cycle_time_ms = prefs.cycle_time_ms;
    pairs[i].ramp_delay_ms = prefs.ramp_delay_ms;
    pairs[i].ramp_step = prefs.ramp_step;
    pairs[i].pair = i;
    pairs[i].direction1 = 1;
    pairs[i].direction2 = -1;
    pairs[i].ttl = 0;

    setScenario(i, prefs.pairs[i].scenario);
    xTaskCreate(TaskFanCycle, "CycleTask", 10000, (void *)&pairs[i], 1, &tasks[i]);
    delay(500);
  }
  PRINT("IP Address: ");
  PRINTLN(WiFi.localIP());
  setup_routing();
}

void loop()
{
  server.handleClient();
}
