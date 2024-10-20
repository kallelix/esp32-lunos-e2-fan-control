#include <Arduino.h>
#include <WebServer.h>
//#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "pinout.h"

// Web server running on port 80
WebServer server(80);
JsonDocument jsonDocument;
char buffer[250];

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
const int DEFAULT_CYCLE_DELAY_MS = 15000;

Scenario off = {DEFAULT_RAMP_DELAY_MS, 1, 0, DEFAULT_CYCLE_DELAY_MS};
Scenario highest = {DEFAULT_RAMP_DELAY_MS, 1, 100, DEFAULT_CYCLE_DELAY_MS};
Scenario high = {DEFAULT_RAMP_DELAY_MS, 1, 70, DEFAULT_CYCLE_DELAY_MS};
Scenario low = {DEFAULT_RAMP_DELAY_MS, 1, 30, DEFAULT_CYCLE_DELAY_MS};

TaskHandle_t Task_living_room;
Scenario Scenario_living_room;

void TaskFanCycle(void *parameter);
void setSpeed(pwm_channel_pair pair, int relativspeed);
void getFan();
void handlePost();

void setup_routing() {     
  //server.on("/temperature", getTemperature);     
  //server.on("/pressure", getPressure);     
  //server.on("/humidity", getHumidity);     
  server.on("/fan", getFan);     
  server.on("/set", HTTP_POST, handlePost);    
          
  server.begin();    
}

void create_json(const char *tag, const float value, const char *unit) {  
  jsonDocument.clear();
  jsonDocument["type"] = tag;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);  
}

void add_json_object(const char *tag, const float value, const char *unit) {
  JsonObject obj = jsonDocument.add<JsonObject>();
  obj["type"] = tag;
  obj["value"] = value;
  obj["unit"] = unit; 
}

void getFan() {
  Serial.println("Get Fan Data");
  jsonDocument.clear();
  add_json_object("power", Scenario_living_room.power, "%");
  add_json_object("cycle", Scenario_living_room.cycle_time_ms, "ms");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void handlePost() {
  if (server.hasArg("plain") == false) {
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);

  int power = jsonDocument["power"];
  int cycle = jsonDocument["cycle"];
  
  Scenario_living_room.power = power;
  Scenario_living_room.cycle_time_ms = cycle;

  server.send(200, "application/json", "{}");
}

void setup()
{

  Serial.begin(9600);
  Serial.println("Starting...");
  WiFiManager wm;
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
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  setup_routing();
}

void TaskFanCycle(void *parameter)
{
  Scenario *data = (Scenario *)parameter;
  Serial.print("Task FanCycle running on core ");
  Serial.println(xPortGetCoreID());
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
  server.begin();
}

// set relativ speed: -100 to 100
void setSpeed(pwm_channel_pair pair, int relativspeed)
{
  int dutyCycle = int(float(relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  int dutyCycleReverse = int(float(-1 * relativspeed + 100) / 200.0 * MAX_DUTY_CYCLE);
  
  ledcWrite(pair.channel_1, dutyCycle);
  ledcWrite(pair.channel_2, dutyCycleReverse);
  Serial.print("Relativ: ");
  Serial.print(relativspeed);
  Serial.print("Max Duty: ");
  Serial.print(MAX_DUTY_CYCLE);
  Serial.print(" set pin1: ");
  Serial.print(pair.channel_1);
  Serial.print(" set duty1: ");
  Serial.print(dutyCycle);
  Serial.print(" read duty1: ");
  Serial.print(ledcRead(pair.channel_1));
  Serial.print(" set pin2: ");
  Serial.print(pair.channel_2);
  Serial.print(" duty2: ");
  Serial.print(dutyCycleReverse);
  Serial.print(" read duty2: ");
  Serial.println(ledcRead(pair.channel_2));
  }

void loop()
{
  server.handleClient(); 
}
