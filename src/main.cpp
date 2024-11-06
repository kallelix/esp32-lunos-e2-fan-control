#include "main.h"
#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include "pinout.h"
#include "prefs.h"
#include "cycles/default/default.h"

// Web server running on port 80
WebServer server(80);

TaskHandle_t Task_living_room;
Scenario Scenario_living_room;

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
  savePrefs(doc);
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
  clearPrefs();
  server.send(200, "application/json", "{}");
  }
}

void setup()
{
  loadPrefs();

  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  PRINTLN("Starting...");
  WiFiManager wm;
  wm.setHostname(prefs.hostname); 
  wm.autoConnect("AutoConnectAP","password");

  // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits)
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  for(int i = 0;i<prefs.no_pairs;i++) {
    ledcSetup(prefs.pairs[i].channel1, PWM_FREQ, PWM_RESOLUTION);
    ledcSetup(prefs.pairs[i].channel2, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(prefs.pairs[i].pin1, prefs.pairs[i].channel1);
    ledcAttachPin(prefs.pairs[i].pin2, prefs.pairs[i].channel2);
  }


  // init living room
for(int i = 0;i<prefs.no_pairs;i++) {
  Scenario_living_room = highest;
  Scenario_living_room.pair = 0;

  xTaskCreate(cycle_default::TaskFanCycle, "CycleTask", 10000, (void *)&Scenario_living_room, 1, &Task_living_room);
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
