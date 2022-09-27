#ifndef _OTA_H
#define _OTA_H

#if !defined(DISABLE_WEB_OTA)
#include <WebServer.h>
#endif

#if defined(DISABLE_WEB_OTA)
  typedef enum {OTA_ARDUINO} OTA_t;
#elif defined(DISABLE_ARDUINO_OTA)
  typedef enum {OTA_WEB} OTA_t;
#else
  typedef enum {OTA_ARDUINO, OTA_WEB} OTA_t;
#endif
  typedef enum {OTA_WIFI_AP, OTA_WIFI_CLIENT} OTA_WiFi_t;
  typedef struct {
    OTA_t otatype;
    OTA_WiFi_t otawifitype;
    const char *ssid;
    const char *key;
    const char *ap_ip;
    const char *device_hostname;
    const char* otapassword;
    const char* appname;
    const char *appversion;
    void (*cb)(const char* param);
  } OTA_Config_t;

  class MyOwnOTA
  {
    public:
      MyOwnOTA();
      void setup(OTA_Config_t *otaconfig);
      void handle(void);
      void OTA_connectToWifi(TimerHandle_t xTimer);

    private:
      void OTA_WiFi_AP_Event(WiFiEvent_t event);
      void OTA_WiFi_AP_setup(void);
      void OTA_WiFi_Client_Event(WiFiEvent_t event);
      void OTA_WiFi_Client_setup(void);
      void OTA_Wifi_disconnect(void);
      void handle_favicon(void);
      void handle_notfound(void);
      void handle_update(void);
      void handle_upload_rqt(void);
      void handle_file_upload(void);
      void OTAWebUpdater_setup(void);
      void handle_arduinoota_end(void);
      void ArduinoOTA_setup(void);

      OTA_Config_t *_otaconfig = NULL;
      TimerHandle_t _OTAwifiReconnectTimer;
      #ifndef DISABLE_WEB_OTA
      boolean _authenticated = false;
      WebServer *_OTA_server;
      #endif
  };
#endif // _OTA_H