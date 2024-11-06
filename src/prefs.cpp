#include "prefs.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>

const char * NAMESPACE = "lunos-fan";

Preferences preferences;

void loadPrefs() {
  preferences.begin(NAMESPACE, false);
  prefs.hostname = preferences.getString("hostname","lunos-fan-control");
  preferences.end();
}

void clearPrefs() {
  preferences.begin(NAMESPACE, false);
  prefs.hostname = preferences.clear();
  preferences.end();
}

void savePrefs(JsonDocument doc) {
  preferences.begin(NAMESPACE, false);
  
  if (!doc["hostname"].isNull()) {
    String hostname = doc["hostname"];
    preferences.putString("hostname", hostname);
    prefs.hostname = hostname;
    WiFiManager wm;
    wm.setHostname(hostname); 
  }
  
  preferences.end();  
}
