#ifndef DISABLE_WEB_OTA
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <Update.h>
#endif
#ifndef DISABLE_ARDUINO_OTA
  #include <ArduinoOTA.h>
  #include <SPIFFS.h>
#endif
#include <map>
#include "ota.h"

/*
std::map<int, const char *> systemEventWifiCode {
  {SYSTEM_EVENT_WIFI_READY,         "SYSTEM_EVENT_WIFI_READY < ESP32 WiFi ready"},
  {SYSTEM_EVENT_SCAN_DONE,              "SYSTEM_EVENT_SCAN_DONE < ESP32 finish scanning AP"},
  {SYSTEM_EVENT_STA_START,              "SYSTEM_EVENT_STA_START < ESP32 station start"},
  {SYSTEM_EVENT_STA_STOP,               "SYSTEM_EVENT_STA_STOP < ESP32 station stop"},
  {SYSTEM_EVENT_STA_CONNECTED,          "SYSTEM_EVENT_STA_CONNECTED < ESP32 station connected to AP"},
  {SYSTEM_EVENT_STA_DISCONNECTED,       "SYSTEM_EVENT_STA_DISCONNECTED < ESP32 station disconnected from AP"},
  {SYSTEM_EVENT_STA_AUTHMODE_CHANGE,    "SYSTEM_EVENT_STA_AUTHMODE_CHANGE < the auth mode of AP connected by ESP32 station changed"},
  {SYSTEM_EVENT_STA_GOT_IP,             "SYSTEM_EVENT_STA_GOT_IP < ESP32 station got IP from connected AP"},
  {SYSTEM_EVENT_STA_LOST_IP,            "SYSTEM_EVENT_STA_LOST_IP < ESP32 station lost IP and the IP is reset to 0"},
  {SYSTEM_EVENT_STA_BSS_RSSI_LOW,       "SYSTEM_EVENT_STA_BSS_RSSI_LOW < ESP32 station connected BSS rssi goes below threshold"},
  {SYSTEM_EVENT_STA_WPS_ER_SUCCESS,     "SYSTEM_EVENT_STA_WPS_ER_SUCCESS < ESP32 station wps succeeds in enrollee mode"},
  {SYSTEM_EVENT_STA_WPS_ER_FAILED,      "SYSTEM_EVENT_STA_WPS_ER_FAILED < ESP32 station wps fails in enrollee mode"},
  {SYSTEM_EVENT_STA_WPS_ER_TIMEOUT,     "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT < ESP32 station wps timeout in enrollee mode"},
  {SYSTEM_EVENT_STA_WPS_ER_PIN,         "SYSTEM_EVENT_STA_WPS_ER_PIN < ESP32 station wps pin code in enrollee mode"},
  {SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP, "SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP < ESP32 station wps overlap in enrollee mode"},
  {SYSTEM_EVENT_AP_START,               "SYSTEM_EVENT_AP_START < ESP32 soft-AP start"},
  {SYSTEM_EVENT_AP_STOP,                "SYSTEM_EVENT_AP_STOP < ESP32 soft-AP stop"},
  {SYSTEM_EVENT_AP_STACONNECTED,        "SYSTEM_EVENT_AP_STACONNECTED < a station connected to ESP32 soft-AP"},
  {SYSTEM_EVENT_AP_STADISCONNECTED,     "SYSTEM_EVENT_AP_STADISCONNECTED < a station disconnected from ESP32 soft-AP"},
  {SYSTEM_EVENT_AP_STAIPASSIGNED,       "SYSTEM_EVENT_AP_STAIPASSIGNED < ESP32 soft-AP assign an IP to a connected station"},
  {SYSTEM_EVENT_AP_PROBEREQRECVED,      "SYSTEM_EVENT_AP_PROBEREQRECVED < Receive probe request packet in soft-AP interface"},
  {SYSTEM_EVENT_ACTION_TX_STATUS,       "SYSTEM_EVENT_ACTION_TX_STATUS < Receive status of Action frame transmitted"},
  {SYSTEM_EVENT_ROC_DONE,               "SYSTEM_EVENT_ROC_DONE < Indicates the completion of Remain-on-Channel operation status"},
  {SYSTEM_EVENT_STA_BEACON_TIMEOUT,     "SYSTEM_EVENT_STA_BEACON_TIMEOUT < ESP32 station beacon timeout"},
  {SYSTEM_EVENT_FTM_REPORT,             "SYSTEM_EVENT_FTM_REPORT < Receive report of FTM procedure"},
  {SYSTEM_EVENT_GOT_IP6,                "SYSTEM_EVENT_GOT_IP6 < ESP32 station or ap or ethernet interface v6IP addr is preferred"},
  {SYSTEM_EVENT_ETH_START,              "SYSTEM_EVENT_ETH_START < ESP32 ethernet start"},
  {SYSTEM_EVENT_ETH_STOP,               "SYSTEM_EVENT_ETH_STOP < ESP32 ethernet stop"},
  {SYSTEM_EVENT_ETH_CONNECTED,          "SYSTEM_EVENT_ETH_CONNECTED < ESP32 ethernet phy link up"},
  {SYSTEM_EVENT_ETH_DISCONNECTED,       "SYSTEM_EVENT_ETH_DISCONNECTED < ESP32 ethernet phy link down"},
  {SYSTEM_EVENT_ETH_GOT_IP,             "SYSTEM_EVENT_ETH_GOT_IP < ESP32 ethernet got IP from connected AP"},
  {SYSTEM_EVENT_ETH_LOST_IP,            "SYSTEM_EVENT_ETH_LOST_IP < ESP32 ethernet lost IP and the IP is reset to 0"}
};
*/
std::map<int, const char *> arduinoEventWifiCode {
  {ARDUINO_EVENT_WIFI_READY,                "ARDUINO_EVENT_WIFI_READY"},
  {ARDUINO_EVENT_WIFI_SCAN_DONE,            "ARDUINO_EVENT_WIFI_SCAN_DONE"},
  {ARDUINO_EVENT_WIFI_STA_START,            "ARDUINO_EVENT_WIFI_STA_START"},
  {ARDUINO_EVENT_WIFI_STA_STOP,             "ARDUINO_EVENT_WIFI_STA_STOP"},
  {ARDUINO_EVENT_WIFI_STA_CONNECTED,        "ARDUINO_EVENT_WIFI_STA_CONNECTED"},
  {ARDUINO_EVENT_WIFI_STA_DISCONNECTED,     "ARDUINO_EVENT_WIFI_STA_DISCONNECTED"},
  {ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE,  "ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE"},
  {ARDUINO_EVENT_WIFI_STA_GOT_IP,           "ARDUINO_EVENT_WIFI_STA_GOT_IP"},
  {ARDUINO_EVENT_WIFI_STA_GOT_IP6,          "ARDUINO_EVENT_WIFI_STA_GOT_IP6"},
  {ARDUINO_EVENT_WIFI_STA_LOST_IP,          "ARDUINO_EVENT_WIFI_STA_LOST_IP"},
  {ARDUINO_EVENT_WIFI_AP_START,             "ARDUINO_EVENT_WIFI_AP_START"},
  {ARDUINO_EVENT_WIFI_AP_STOP,              "ARDUINO_EVENT_WIFI_AP_STOP"},
  {ARDUINO_EVENT_WIFI_AP_STACONNECTED,      "ARDUINO_EVENT_WIFI_AP_STACONNECTE"},
  {ARDUINO_EVENT_WIFI_AP_STADISCONNECTED,   "ARDUINO_EVENT_WIFI_AP_STADISCONNECTED"},
  {ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED,     "ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED"},
  {ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED,    "ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED"},
  {ARDUINO_EVENT_WIFI_AP_GOT_IP6,           "ARDUINO_EVENT_WIFI_AP_GOT_IP6"},
  {ARDUINO_EVENT_WIFI_FTM_REPORT,           "ARDUINO_EVENT_WIFI_FTM_REPORT"},
  {ARDUINO_EVENT_ETH_START,                 "ARDUINO_EVENT_ETH_START"},
  {ARDUINO_EVENT_ETH_STOP,                  "ARDUINO_EVENT_ETH_STOP"},
  {ARDUINO_EVENT_ETH_CONNECTED,             "ARDUINO_EVENT_ETH_CONNECTED"},
  {ARDUINO_EVENT_ETH_DISCONNECTED,          "ARDUINO_EVENT_ETH_DISCONNECTED"},
  {ARDUINO_EVENT_ETH_GOT_IP,                "ARDUINO_EVENT_ETH_GOT_IP"},
  {ARDUINO_EVENT_ETH_GOT_IP6,               "ARDUINO_EVENT_ETH_GOT_IP6"},
  {ARDUINO_EVENT_WPS_ER_SUCCESS,            "ARDUINO_EVENT_WPS_ER_SUCCESS"},
  {ARDUINO_EVENT_WPS_ER_FAILED,             "ARDUINO_EVENT_WPS_ER_FAILED"},
  {ARDUINO_EVENT_WPS_ER_TIMEOUT,            "ARDUINO_EVENT_WPS_ER_TIMEOUT"},
  {ARDUINO_EVENT_WPS_ER_PIN,                "ARDUINO_EVENT_WPS_ER_PIN"},
  {ARDUINO_EVENT_WPS_ER_PBC_OVERLAP,        "ARDUINO_EVENT_WPS_ER_PBC_OVERLAP"},
  {ARDUINO_EVENT_SC_SCAN_DONE,              "ARDUINO_EVENT_SC_SCAN_DONE"},
  {ARDUINO_EVENT_SC_FOUND_CHANNEL,          "ARDUINO_EVENT_SC_FOUND_CHANNEL"},
  {ARDUINO_EVENT_SC_GOT_SSID_PSWD,          "ARDUINO_EVENT_SC_GOT_SSID_PSWD"},
  {ARDUINO_EVENT_SC_SEND_ACK_DONE,          "ARDUINO_EVENT_SC_SEND_ACK_DONE"},
  {ARDUINO_EVENT_PROV_INIT,                 "ARDUINO_EVENT_PROV_INIT"},
  {ARDUINO_EVENT_PROV_DEINIT,               "ARDUINO_EVENT_PROV_DEINIT"},
  {ARDUINO_EVENT_PROV_START,                "ARDUINO_EVENT_PROV_START"},
  {ARDUINO_EVENT_PROV_END,                  "ARDUINO_EVENT_PROV_END"},
  {ARDUINO_EVENT_PROV_CRED_RECV,            "ARDUINO_EVENT_PROV_CRED_RECV"},
  {ARDUINO_EVENT_PROV_CRED_FAIL,            "ARDUINO_EVENT_PROV_CRED_FAIL"},
  {ARDUINO_EVENT_PROV_CRED_SUCCESS,         "ARDUINO_EVENT_PROV_CRED_SUCCESS"}
};


static void StaticTimerCallbackFunction(TimerHandle_t xTimer) {
  MyOwnOTA* me = static_cast<MyOwnOTA*>(pvTimerGetTimerID(xTimer));
  me->OTA_connectToWifi(xTimer);
}

MyOwnOTA::MyOwnOTA() {
#ifndef DISABLE_WEB_OTA
  WebServer OTA_server(80);
  _OTA_server = &OTA_server;
#endif
}

// Private functions
#define debug Serial.printf

///////////////////////////////////////////////////////////////////////
// WiFi AP Stuff
///////////////////////////////////////////////////////////////////////

void MyOwnOTA::OTA_WiFi_AP_Event(WiFiEvent_t event) {
  //debug("[WiFi-event] event: %d\n", event);
  //debug("[System  WiFi-event]: %d > %s\n", event, systemEventWifiCode[event]);
  debug("[Arduino WiFi-event]: %d > %s\n", event, arduinoEventWifiCode[event]);
  switch(event) {
  case ARDUINO_EVENT_WIFI_AP_START:
      debug("WiFi AP started\n");
      debug("IP address: %s\n", WiFi.softAPIP().toString().c_str());
      debug("Network ID: %s\n", WiFi.softAPNetworkID().toString().c_str());
      debug("Broadcast IP: %s\n", WiFi.softAPBroadcastIP().toString().c_str());
      if (_otaconfig->cb !=NULL)
        (*_otaconfig->cb)(WiFi.softAPIP().toString().c_str());
      break;
  case ARDUINO_EVENT_WIFI_AP_STOP:
      debug("WiFi AP stopped\n");
      break;
  case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      debug("a station connected to ESP32 soft-AP\n");
      debug("%d client(s) connected.\n", WiFi.softAPgetStationNum());
      break;
  case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      debug("a station disconnected from ESP32 soft-AP\n");
      debug("%d client(s) connected.\n", WiFi.softAPgetStationNum());
      break;
  case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
      debug("ESP32 soft-AP assign an IP to a connected station\n");
      break;
  default:
      break;
  }
}
void MyOwnOTA::OTA_WiFi_AP_setup(void) {
  IPAddress ap_ip;
  debug("Starting OTA Wi-Fi AP...\n");
  WiFi.onEvent(std::bind(&MyOwnOTA::OTA_WiFi_AP_Event, this, std::placeholders::_1));
  if (_otaconfig->ap_ip != NULL)
    ap_ip.fromString(_otaconfig->ap_ip);
  else
    ap_ip.fromString("192.168.4.1");
  WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255,255,255,0));
  WiFi.softAPsetHostname(_otaconfig->device_hostname);
  WiFi.softAP(_otaconfig->device_hostname);
  debug("IP address: %s\n", WiFi.softAPIP().toString().c_str());
}

///////////////////////////////////////////////////////////////////////
// WiFi Client Stuff
///////////////////////////////////////////////////////////////////////


void MyOwnOTA::OTA_connectToWifi(TimerHandle_t xTimer) {
  debug("Connecting to Wi-Fi...\n");
  //WiFi.begin(_otaconfig->ssid, _otaconfig->key);
  Serial.println("Timer is OK !");
}


void MyOwnOTA::OTA_WiFi_Client_Event(WiFiEvent_t event) {
  //debug("[WiFi-event] event: %d\n", event);
  //debug("[System  WiFi-event]: %d > %s\n", event, systemEventWifiCode[event]);
  debug("[Arduino WiFi-event]: %d > %s\n", event, arduinoEventWifiCode[event]);
  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      debug("WiFi connected\n");
      debug("IP address: %s\n", WiFi.localIP().toString().c_str());
      if (_otaconfig->cb !=NULL)
        (*_otaconfig->cb)(WiFi.localIP().toString().c_str());
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      debug("WiFi lost connection\n");
      xTimerStart(_OTAwifiReconnectTimer, 0);
      break;
    default:
      break;
  }
}


void MyOwnOTA::OTA_WiFi_Client_setup() {
  _OTAwifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, this, reinterpret_cast<TimerCallbackFunction_t>(StaticTimerCallbackFunction));
  WiFi.onEvent(std::bind(&MyOwnOTA::OTA_WiFi_Client_Event, this, std::placeholders::_1));
  OTA_connectToWifi(NULL);

  while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
  }
}

///////////////////////////////////////////////////////////////////////
// WiFi Client & AP Stuff
///////////////////////////////////////////////////////////////////////

void MyOwnOTA::OTA_Wifi_disconnect() {
  switch(_otaconfig->otawifitype) {
    case OTA_WIFI_AP:
      WiFi.softAPdisconnect();
      break;
    case OTA_WIFI_CLIENT:
      WiFi.disconnect();
      break;
  }
}

///////////////////////////////////////////////////////////////////////
// OTA Web Update Stuff
///////////////////////////////////////////////////////////////////////
#ifndef DISABLE_WEB_OTA
static const String updateIndex1 =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<p>";
/*
"APP_NAME"
" "
"APP_VERSION"
*/
static const String updateIndex2 =
" OTA Update"
"</p>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
"</form>"
"<div id='prg'></div>"
"<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/upload',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('load', function(event) {$('#prg').html('<p>Update: success !</p><p>Rebooting...</p>');});"
  "xhr.upload.addEventListener('error', function(event) {$('#prg').html('Update: an error occurred.');});"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('Success!');"
  "},"
  "error: function (a, b, c) {"
  "console.log('Error!');"
  "}"
  "});"
  "});"
"</script>";

  // allows you to set the realm of authentication Default:"Login Required"
  static const char* www_realm = "ESP32 OTA Update";
  // the Content of the HTML response in case of Unautherized Access Default:empty
  static String authFailResponse = "Authentication Failed";

void MyOwnOTA::handle_favicon(void) {
  _OTA_server->sendHeader("Connection", "close");
  _OTA_server->send(200, "text/html", "<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">");
}

void MyOwnOTA::handle_notfound(void) {
      String message = "Server is running!\n\n";
      message += "URI: ";
      message +=  _OTA_server->uri();
      message += "\nMethod: ";
      message += (_OTA_server->method() == HTTP_GET) ? "GET" : "POST";
      message += "\nArguments: ";
      message +=  _OTA_server->args();
      message += "\n";
      _OTA_server->send(200, "text/plain", message);
}

void MyOwnOTA::handle_update(void) {
  if (!_OTA_server->authenticate("MyOwnOTA", _otaconfig->otapassword))
    //Basic Auth Method with Custom realm and Failure Response
    //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
    //Digest Auth Method with realm="Login Required" and empty Failure Response
    //return server.requestAuthentication(DIGEST_AUTH);
    //Digest Auth Method with Custom realm and empty Failure Response
    //return server.requestAuthentication(DIGEST_AUTH, www_realm);
    //Digest Auth Method with Custom realm and Failure Response
  {
    return _OTA_server->requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
  }
  _authenticated = true;
  _OTA_server->sendHeader("Connection", "close");
  _OTA_server->send(200, "text/html", updateIndex1+_otaconfig->appname+" "+_otaconfig->appversion+updateIndex2);
}

void MyOwnOTA::handle_upload_rqt(void) {
  _OTA_server->sendHeader("Connection", "close");
  _OTA_server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  delay(3000);
  ESP.restart();
}

void MyOwnOTA::handle_file_upload(void) {
  HTTPUpload& upload = _OTA_server->upload();
  if (upload.status == UPLOAD_FILE_START) {
      debug("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
          //Update.printError(Serial);
          debug(Update.errorString());
      }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          //Update.printError(Serial);
          debug(Update.errorString());
      }
  } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
          //Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          debug("Update Success: %u bytes\n", upload.totalSize);
          debug("Rebooting...\n");
          //OTAWebUpdaterMessage = "Update Success: " + upload.totalSize;
          //OTAWebUpdaterMessage += " bytes\nRebooting...\n";
          _OTA_server->close();
          OTA_Wifi_disconnect();
          delay (100);
      } else {
          //Update.printError(Serial);
          debug(Update.errorString());
          //OTAWebUpdaterMessage = Update.errorString();
      }
  }
}

void MyOwnOTA::OTAWebUpdater_setup(void) {
  _OTA_server->on("/favicon.ico", HTTP_GET, std::bind(&MyOwnOTA::handle_favicon, this));
  _OTA_server->onNotFound(std::bind(&MyOwnOTA::handle_notfound, this));
  _OTA_server->on("/update", HTTP_GET, std::bind(&MyOwnOTA::handle_update, this));
  /*handling uploading firmware file */
  _OTA_server->on("/upload", HTTP_POST, std::bind(&MyOwnOTA::handle_upload_rqt, this), std::bind(&MyOwnOTA::handle_file_upload, this));

  _OTA_server->begin();
  MDNS.begin(_otaconfig->device_hostname);
  MDNS.addService("http", "tcp", 80);
  Serial.println("OTA update web server started.");
}
#endif

///////////////////////////////////////////////////////////////////////
// OTA Arduino Update Stuff
///////////////////////////////////////////////////////////////////////
#ifndef DISABLE_ARDUINO_OTA
void MyOwnOTA::handle_arduinoota_end(void) {
  ArduinoOTA.end();
  Serial.println("\nEnd");
  delay (100);
  OTA_Wifi_disconnect();
}

void MyOwnOTA::ArduinoOTA_setup(void) {
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  ArduinoOTA.setMdnsEnabled(true);
  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(_otaconfig->device_hostname);
  Serial.printf("Device hostname: %s.local\n", ArduinoOTA.getHostname().c_str());

  // No authentication by default
  //ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  if (_otaconfig->otapassword != NULL)
    ArduinoOTA.setPassword(_otaconfig->otapassword);

  ArduinoOTA
      .onStart([]() {
          String type;
          if (ArduinoOTA.getCommand() == U_FLASH) {
              type = "sketch";
          } else { // U_SPIFFS
              type = "filesystem";
              // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
              SPIFFS.end();
          }
          Serial.println("Start updating " + type);
      })
      .onEnd(std::bind(&MyOwnOTA::handle_arduinoota_end, this))
      .onProgress([](unsigned int progress, unsigned int total) {
          Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
          Serial.printf("Error[%u]: ", error);
          if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
          else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
          else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
          else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
          else if (error == OTA_END_ERROR) Serial.println("End Failed");
      });

  ArduinoOTA.begin();
  Serial.println("OTA update Arduino server started.");
}
#endif

///////////////////////////////////////////////////////////////////////
// Public functions
///////////////////////////////////////////////////////////////////////

void MyOwnOTA::setup(OTA_Config_t *otaconfig) {
  _otaconfig = otaconfig;

  switch(_otaconfig->otawifitype) {
    case OTA_WIFI_AP:
      OTA_WiFi_AP_setup();
      break;
    case OTA_WIFI_CLIENT:
      OTA_WiFi_Client_setup();
      break;
  }


  switch(_otaconfig->otatype) {
#ifndef DISABLE_ARDUINO_OTA
    case OTA_ARDUINO:
      ArduinoOTA_setup();
      break;
#endif
#ifndef DISABLE_WEB_OTA
    case OTA_WEB:
      OTAWebUpdater_setup();
      break;
#endif
  }
}

void MyOwnOTA::handle(void) {
  switch(_otaconfig->otatype) {
#ifndef DISABLE_ARDUINO_OTA
    case OTA_ARDUINO:
      ArduinoOTA.handle();
      break;
#endif
#ifndef DISABLE_WEB_OTA
    case OTA_WEB:
      _OTA_server->handleClient();
      break;
#endif
  }
}
