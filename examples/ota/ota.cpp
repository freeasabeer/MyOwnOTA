#include <Arduino.h>
#include "ota.h"

#define WIFI_SSID "MyOwnSSID"
#define WIFI_KEY "MyOwnKey"
#define APP_NAME "MyApp"
#define APP_VERSION "0.1"
#define DEVICE_HOSTNAME "MyApp"

static int pinOTAvalue;
void display_ota_ip(const char *ip);

OTA_Config_t otaconfig = {
  //OTA_WEB,               // Either OTA_WEB or OTA_ARDUINO
  OTA_ARDUINO,
  OTA_WIFI_AP, "192.168.16.1", NULL, NULL, // Either OTA_WIFI_AP or OTA_WIFI_CLIENT
  //OTA_WIFI_CLIENT, NULL, WIFI_SSID, WIFI_KEY,
  DEVICE_HOSTNAME,
  "admin",
  APP_NAME,
  APP_VERSION,
  &display_ota_ip
};
MyOwnOTA ota;


void setup() {
  Serial.begin(115200);

  pinMode(27, INPUT_PULLUP);
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif
  pinOTAvalue = digitalRead(27);
  if (pinOTAvalue == LOW) {
    // Switch to OTA update mode
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, HIGH);
#endif
    ota.setup(&otaconfig);
    return;
  }
  // Put your own application set-up code after this line

}


void loop() {
  if (pinOTAvalue == LOW) {
    ota.handle();
    return;
  }

  // Put your own application loop code after this line


}

void display_ota_ip(const char *ip) {

  // Put your own IP address display code after this line
  // It can be a Serial print to the console (which is already done by the MyOwnOTA library),
  // a print out on a LCD display or whatever else display.
  // It can be through a message sent to Telegram, to a Gotify server: possibilties are endless
  Serial.printf("Ready for OTA connection on: %s/n", ip);
  
}