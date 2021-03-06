#include "StandStats.h"
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <Time.h>
#include <TimeLib.h>
#include <Preferences.h>
#include <driver/adc.h>
#include "WiFi.h"

Adafruit_7segment matrix = Adafruit_7segment();
Preferences preferences;

const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;
float standingMaxVolt = STANDING_MAX_VOLT;
float presenceMinVolt = PRESENCE_MIN_VOLT;
int standingCount = 0;
int presenceCount = 0;
time_t lastCheckTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  setupWifi();
  setupTime();
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
  float standingVoltage = get_voltage(STANDING_SENSOR, 10) * (2.2 / 4095.0);
  float presenceVoltage = get_voltage(PRESENCE_SENSOR, 10) * (2.2 / 4095.0);
  float standingRatio = 0.0;
  lastCheckTime = now();
  printLocalTime();

  // Likely not there, don't count anything
  Serial.println(presenceVoltage);
  if(presenceVoltage < presenceMinVolt) {
    Serial.println("Not present");
    matrix.clear();
    matrix.writeDisplay();
    delay(500);
    return;
  }  

  presenceCount = presenceCount + timeSpan;

  Serial.println(standingVoltage);
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
  delay(500);
}

float get_voltage(adc1_channel_t pin, int samples) {
  float total = 0;

  for(int i = 0; i < samples; i++){
    total += adc1_get_voltage(pin);
  }

  return total / samples;
}

void setupWifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupTime() {
  long timezone = -5; 
  byte daysavetime = 1;
  configTime(3600 * timezone, daysavetime * 3600, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
}

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
