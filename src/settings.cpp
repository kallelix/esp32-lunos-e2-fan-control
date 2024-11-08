#include "settings.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>

const char *NAMESPACE = "fan";

Preferences preferences;

Prefs prefs = {"lunos-fan-control", 0, {{"pair1",0,1,3,4,SCENARIO_DEFAULT},{"pair2",2,3,0,0,SCENARIO_DEFAULT},{"pair3",4,5,0,0,SCENARIO_DEFAULT},{"pair4",6,7,0,0,SCENARIO_DEFAULT},{"pair5",8,9,0,0,SCENARIO_DEFAULT}}, DEFAULT_RAMP_DELAY_MS, DEFAULT_RAMP_STEP, DEFAULT_CYCLE_DELAY_MS};

void loadPrefs()
{
  preferences.begin(NAMESPACE, true);
  prefs.hostname = preferences.getString("hostname", "lunos-fan-control");
  prefs.no_pairs =preferences.getInt("no_pairs", 0);
  for (int i = 0; i < prefs.no_pairs; i++)
  {
    prefs.pairs[i].name = preferences.getString(("pairname" + String(i)).c_str(), "pair" + String(i));
    prefs.pairs[i].pin1 = preferences.getInt(("fanpin1" + String(i)).c_str(), 0);
    prefs.pairs[i].pin2 = preferences.getInt(("fanpin2" + String(i)).c_str(), 0);
    prefs.pairs[i].scenario = preferences.getInt(("defaultscenario" + String(i)).c_str(), SCENARIO_DEFAULT);
  }
  preferences.end();
}

void clearPrefs()
{
  preferences.begin(NAMESPACE, false);
  prefs.hostname = preferences.clear();
  preferences.end();
}

int switchScenario(String scenario)
{
  if (scenario == "default")
  {
    return SCENARIO_DEFAULT;
  }
  else if (scenario == "highest")
  {
    return SCENARIO_HIGHEST;
  }
  else if (scenario == "high")
  {
    return SCENARIO_HIGH;
  }
  else if (scenario == "medium")
  {
    return SCENARIO_MID;
  }
  else if (scenario == "low")
  {
    return SCENARIO_LOW;
  }
  else if (scenario == "off")
  {
    return SCENARIO_OFF;
  }
  else if (scenario == "summernight")
  {
    return SCENARIO_SUMMER_NIGHT;
  }
  else if (scenario == "night")
  {
    return SCENARIO_NIGHT;
  }
  else if (scenario == "out")
  {
    return SCENARIO_OUT;
  }
  else if (scenario == "in")
  {
    return SCENARIO_IN;
  }
  else if (scenario == "shortshootout")
  {
    return SCENARIO_SHORT_SHOOTOUT;
  }
  return SCENARIO_DEFAULT;
}

String savePrefs(JsonDocument doc)
{
  String result = "nop";
  JsonDocument rdoc;
  
  preferences.begin(NAMESPACE, false);

  if (!doc["hostname"].isNull())
  {
    String hostname = doc["hostname"];
    prefs.hostname = hostname;
    preferences.putString("hostname", hostname);
    WiFiManager wm;
    wm.setHostname(hostname);
    rdoc["hostname"] = hostname;
  }
  if (!doc["fanpairs"].isNull())
  {
    int size = doc["fanpairs"].size();
    prefs.no_pairs = size;
    preferences.putInt("no_pairs", prefs.no_pairs);
    rdoc["no_pairs"] = size;
    for (int i = 0; i < size; i++)
    {
      String pairname = doc["fanpairs"][i]["name"];
      prefs.pairs[i].name = pairname;
      prefs.pairs[i].pin1 = doc["fanpairs"][i]["fanpin1"];
      prefs.pairs[i].pin2 = doc["fanpairs"][i]["fanpin2"];
      String scenario = doc["fanpairs"][i]["defaultscenario"];
      prefs.pairs[i].scenario = switchScenario(scenario);
      preferences.putString(("pairname" + String(i)).c_str(), pairname);
      preferences.putInt(("fanpin1" + String(i)).c_str(), prefs.pairs[i].pin1);
      preferences.putInt(("fanpin2" + String(i)).c_str(), prefs.pairs[i].pin2);
      preferences.putInt(("defaultscenario" + String(i)).c_str(), prefs.pairs[i].scenario);
      rdoc["pairs"][i]["name"] = pairname;
      rdoc["pairs"][i]["fanpin1"] = prefs.pairs[i].pin1;
      rdoc["pairs"][i]["fanpin2"] = prefs.pairs[i].pin2;
      rdoc["pairs"][i]["defaultscenario"] = scenario;
    }
  }

  preferences.end();
  serializeJson(rdoc, result);
  return result;
}
