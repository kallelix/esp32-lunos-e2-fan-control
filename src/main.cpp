#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "pinout.h"
#include <Preferences.h>

#define DEBUG

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

// Web server running on port 80
WebServer server(80);
Preferences preferences;
String hostname;

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

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
  // direction modifier fan1
  int direction1;
  // direction modifier fan2
  int direction2;

  pwm_channel_pair pair;

} Scenario;

const int DEFAULT_RAMP_DELAY_MS = 40; // delay between fade increments
const int DEFAULT_CYCLE_DELAY_MS = 30000;

Scenario off = {DEFAULT_RAMP_DELAY_MS, 4, 0, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario highest = {DEFAULT_RAMP_DELAY_MS, 4, 100, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario high = {DEFAULT_RAMP_DELAY_MS, 4, 70, DEFAULT_CYCLE_DELAY_MS, 1, -1};
Scenario low = {DEFAULT_RAMP_DELAY_MS, 4, 30, DEFAULT_CYCLE_DELAY_MS, 1, -1};

TaskHandle_t Task_living_room;
Scenario Scenario_living_room;

void TaskFanCycle(void *parameter);
void setSpeed(pwm_channel_pair pair, int relativspeed);
void getInfo();
void handleSet();
void handleConfig();
void handleClear();

void setup_routing() {     
  server.on("/info", getInfo);     
  server.on("/set", HTTP_POST, handleSet);    
  server.on("/config", HTTP_POST, handleConfig);    
  server.on("/clear", HTTP_POST, handleClear);    
          
  server.begin();    
}

void add_json_object(JsonDocument& doc, const char *tag, const float value, const char *unit) {
  JsonObject obj = doc.add<JsonObject>();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getInfo() {
  String json;
  JsonDocument doc;
  PRINTLN("Get Fan Data");
  add_json_object(doc, "power", Scenario_living_room.power, "%");
  add_json_object(doc, "cycle", Scenario_living_room.cycle_time_ms, "ms");
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleSet() {
  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{}");
  }
  else
  {
  JsonDocument doc;
  String body = server.arg("plain");
  deserializeJson(doc, body);

  int power = doc["power"];
  int cycle = doc["cycle"];
  
  Scenario_living_room.power = power;
  Scenario_living_room.cycle_time_ms = cycle;

  server.send(200, "application/json", "{}");
  }
}

void handleConfig() {
  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{}");
  }
  else
  {
  JsonDocument doc;  
  String body = server.arg("plain");
  deserializeJson(doc, body);
  /*
  int power = jsonDocument["power"];
  int cycle = jsonDocument["cycle"];
  
  Scenario_living_room.power = power;
  Scenario_living_room.cycle_time_ms = cycle;
  */
  server.send(200, "application/json", "{}");
  }
}

void handleClear() {
  if (server.hasArg("plain") == false) {
    server.send(400, "application/json", "{}");
  }
  else
  {
  JsonDocument doc;
  String body = server.arg("plain");
  deserializeJson(doc, body);
  /*
  int power = jsonDocument["power"];
  int cycle = jsonDocument["cycle"];
  
  Scenario_living_room.power = power;
  Scenario_living_room.cycle_time_ms = cycle;
  */
  server.send(200, "application/json", "{}");
  }
}

void setup()
{
  preferences.begin("lunos-fan", false);
  hostname = preferences.getString("hostname","lunos-fan-control");
  preferences.end();

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  PRINTLN("Starting...");
  WiFiManager wm;
  wm.setHostname(hostname); 
  wm.autoConnect("AutoConnectAP","password");

  // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits)
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  ledcSetup(living_room.channel_1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(living_room.channel_2, PWM_FREQ, PWM_RESOLUTION);
  //ledcSetup(rest_rooms.channel_1, PWM_FREQ, PWM_RESOLUTION);
  //ledcSetup(rest_rooms.channel_2, PWM_FREQ, PWM_RESOLUTION);

  // ledcAttachPin(uint8_t pin, uint8_t channel);
  //ledcAttachPin(LED1_OUTPUT_PIN, living_room.channel_1);
  //ledcAttachPin(LED2_OUTPUT_PIN, living_room.channel_2);
  ledcAttachPin(FAN1_LIVING_ROOM_OUTPUT_PIN, living_room.channel_1);
  ledcAttachPin(FAN2_LIVING_ROOM_OUTPUT_PIN, living_room.channel_2);

  // init living room
  Scenario_living_room = highest;
  Scenario_living_room.pair = living_room;

  xTaskCreate(TaskFanCycle, "CycleLivingRoom", 10000, (void *)&Scenario_living_room, 1, &Task_living_room);
  delay(500);
  PRINT("IP Address: ");
  PRINTLN(WiFi.localIP());
  setup_routing();
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
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = data->power; fade >= (-1 * data->power); fade-=data->ramp_step)
    {
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }

    delay(data->cycle_time_ms);

    for (int fade = (-1 * data->power); fade <= 0; fade+=data->ramp_step)
    {
      setSpeed(data->pair, fade);
      delay(data->ramp_delay_ms);
    }
  }
  server.begin();
}

// set relativ speed: -100 to 100
void setSpeed(pwm_channel_pair pair, int relativspeed, int direction1, int direction2)
{
  int dutyCycle1 = int(float(direction1 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  int dutyCycle2 = int(float(direction2 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  
  ledcWrite(pair.channel_1, dutyCycle1);
  ledcWrite(pair.channel_2, dutyCycle2);
  PRINT("Relativ: ");
  PRINT(relativspeed);
  PRINT("Max Duty: ");
  PRINT(MAX_DUTY_CYCLE);
  PRINT(" set pin1: ");
  PRINT(pair.channel_1);
  PRINT(" set duty1: ");
  PRINT(dutyCycle1);
  PRINT(" set pin2: ");
  PRINT(pair.channel_2);
  PRINT(" duty2: ");
  PRINT(dutyCycle2);
  PRINT(" read duty2: ");
  PRINTLN(ledcRead(pair.channel_2));
  }

void loop()
{
  server.handleClient(); 
}
