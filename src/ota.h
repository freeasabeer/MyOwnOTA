#ifndef _OTA_H
#define _OTA_H

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
    const char *device_hostname;
    const char* otapassword;
    const char* appname;
    const char *appversion;
  } OTA_Config_t;
  void OTA_setup(OTA_Config_t *otaconfig);
  void OTA_handle(void);

#endif // _OTA_H