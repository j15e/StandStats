#include "StandStats.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Time.h>
#include <TimeLib.h>
#include <Preferences.h>
#include <driver/adc.h>

Adafruit_7segment matrix = Adafruit_7segment();
Preferences preferences;

float standingMaxVolt = STANDING_MAX_VOLT;
float presenceMinVolt = PRESENCE_MIN_VOLT;
int standingCount = 0;
int presenceCount = 0;
time_t lastCheckTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  matrix.begin(0x70);
  matrix.setBrightness(0.9);
  lastCheckTime = now();

  // Set analog width and full-scale voltage to 2.2V
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(STANDING_SENSOR, ADC_ATTEN_6db);
  adc1_config_channel_atten(PRESENCE_SENSOR, ADC_ATTEN_6db);
  
  // Load saved stats
  preferences.begin("standstats", false);
  standingCount = preferences.getUInt("standingCount", 0);
  presenceCount = preferences.getUInt("presenceCount", 0);
}

void loop() {
  int timeSpan = now() - lastCheckTime;
  float standingVoltage = adc1_get_voltage(STANDING_SENSOR) * (2.2 / 4095.0);
  float presenceVoltage = adc1_get_voltage(PRESENCE_SENSOR) * (2.2 / 4095.0);
  float standingRatio = 0.0;
  lastCheckTime = now();

  // Likely not there, don't count anything
  if(presenceVoltage < presenceMinVolt) {
    Serial.println("Not present");
    matrix.clear();
    matrix.writeDisplay();
    return;
  }  

  presenceCount = presenceCount + timeSpan;

  if (standingVoltage < standingMaxVolt) {
    standingCount = standingCount + timeSpan;
    Serial.println("Standing");
  } else {
    Serial.println("Sitting"); 
  }
  
  if(presenceCount > 0) {
    standingRatio = (float)standingCount / presenceCount;  
  } else {
    standingRatio = 0;
  }
  
  matrix.print(standingRatio*100);
  matrix.writeDisplay();
  preferences.putUInt("standingCount", standingCount);
  preferences.putUInt("presenceCount", presenceCount);
  delay(1000);
}
