#include <Arduino.h>
#include "ota.h"

#define WIFI_SSID "MyOwnSSID"
#define WIFI_KEY "MyOwnKey"
#define APP_NAME "MyApp"
#define APP_VERSION "0.1"
#define DEVICE_HOSTNAME "MyApp"

static int pinOTAvalue;

OTA_Config_t otaconfig = {
  //OTA_WEB,               // Either OTA_WEB or OTA_ARDUINO
  OTA_ARDUINO,
  OTA_WIFI_AP, NULL, NULL, // Either OTA_WIFI_AP or OTA_WIFI_CLIENT
  //OTA_WIFI_CLIENT, WIFI_SSID, WIFI_KEY,
  DEVICE_HOSTNAME,
  "admin",
  APP_NAME,
  APP_VERSION
};

void setup() {
  Serial.begin(115200);

  pinMode(27, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinOTAvalue = digitalRead(27);
  if (pinOTAvalue == LOW) {
    // Switch to OTA update mode
    digitalWrite(LED_BUILTIN, HIGH);
    OTA_setup(&otaconfig);
    return;
  }
  // Put your own application set-up code after this line

  }
}

void loop() {
  if (pinOTAvalue == LOW) {
    OTA_handle();
    return;
  }

  // Put your own application loop code after this line


}
