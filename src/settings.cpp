#include "settings.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiManager.h>

const char *NAMESPACE = "lunos-fan";

Preferences preferences;

void loadPrefs()
{
  preferences.begin(NAMESPACE, false);
  prefs.hostname = preferences.getString("hostname", "lunos-fan-control");
  for (int i = 0; i < preferences.getInt("no_pairs", 1); i++)
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
  else if (scenario == "summer")
  {
    return SCENARIO_SUMMER;
  }
  else if (scenario == "out")
  {
    return SCENARIO_OUT;
  }
  return SCENARIO_DEFAULT;
}

void savePrefs(JsonDocument doc)
{
  preferences.begin(NAMESPACE, false);

  if (!doc["hostname"].isNull())
  {
    String hostname = doc["hostname"];
    prefs.hostname = hostname;
    preferences.putString("hostname", hostname);
    WiFiManager wm;
    wm.setHostname(hostname);
  }
  if (!doc["fanpairs"].isNull())
  {
    for (int i = 0; i < doc["fanpairs"].size(); i++)
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
    }
    prefs.no_pairs = doc["fanpairs"].size();
    preferences.putInt("no_pairs", prefs.no_pairs);
  }

  preferences.end();
}
