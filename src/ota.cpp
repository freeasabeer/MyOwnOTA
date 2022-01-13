#ifndef DISABLE_WEB_OTA
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <Update.h>
#endif
#ifndef DISABLE_ARDUINO_OTA
  #include <ArduinoOTA.h>
  #include <SPIFFS.h>
#endif
#include "ota.h"

static OTA_Config_t *s_otaconfig = NULL;

// Private functions
#define debug Serial.printf

///////////////////////////////////////////////////////////////////////
// WiFi AP Stuff
///////////////////////////////////////////////////////////////////////

void OTA_WiFi_AP_Event(WiFiEvent_t event) {
  debug("[WiFi-event] event: %d\n", event);
  switch(event) {
  case SYSTEM_EVENT_AP_START:
      debug("WiFi AP started\n");
      debug("IP address: %s\n", WiFi.softAPIP().toString().c_str());
      debug("Network ID: %s\n", WiFi.softAPNetworkID().toString().c_str());
      debug("Broadcast IP: %s\n", WiFi.softAPBroadcastIP().toString().c_str());
      break;
  case SYSTEM_EVENT_AP_STOP:
      debug("WiFi AP stopped\n");
      break;
  case SYSTEM_EVENT_AP_STACONNECTED:
      debug("a station connected to ESP32 soft-AP\n");
      debug("%d client(s) connected.\n", WiFi.softAPgetStationNum());
      break;
  case SYSTEM_EVENT_AP_STADISCONNECTED:
      debug("a station disconnected from ESP32 soft-AP\n");
      debug("%d client(s) connected.\n", WiFi.softAPgetStationNum());
      break;
  case SYSTEM_EVENT_AP_STAIPASSIGNED:
      debug("ESP32 soft-AP assign an IP to a connected station\n");
      break;
  default:
      break;
  }
}
void OTA_WiFi_AP_setup(void) {
  debug("Starting OTA Wi-Fi AP...\n");

  WiFi.onEvent(OTA_WiFi_AP_Event);
  WiFi.softAPConfig(IPAddress(192,168,4,1), IPAddress(192,168,4,1), IPAddress(255,255,255,0));
  WiFi.softAPsetHostname(s_otaconfig->device_hostname);
  WiFi.softAP(s_otaconfig->device_hostname);
  debug("IP address: %s\n", WiFi.softAPIP().toString().c_str());
}

///////////////////////////////////////////////////////////////////////
// WiFi Client Stuff
///////////////////////////////////////////////////////////////////////

static TimerHandle_t OTAwifiReconnectTimer;

void OTA_connectToWifi(TimerHandle_t xTimer) {
  debug("Connecting to Wi-Fi...\n");
  WiFi.begin(s_otaconfig->ssid, s_otaconfig->key);
}


void OTA_WiFi_Client_Event(WiFiEvent_t event) {
  debug("[WiFi-event] event: %d\n", event);
  switch(event) {
  case SYSTEM_EVENT_STA_GOT_IP:
      debug("WiFi connected\n");
      debug("IP address: %s\n", WiFi.localIP().toString().c_str());
      break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
      debug("WiFi lost connection\n");
      xTimerStart(OTAwifiReconnectTimer, 0);
      break;
  default:
      break;
  }
}

void OTA_WiFi_Client_setup() {
  OTAwifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(OTA_connectToWifi));
  WiFi.onEvent(OTA_WiFi_Client_Event);
  OTA_connectToWifi(NULL);

  while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
  }
}

///////////////////////////////////////////////////////////////////////
// WiFi Client & AP Stuff
///////////////////////////////////////////////////////////////////////

void OTA_Wifi_disconnect() {
  switch(s_otaconfig->otawifitype) {
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
static boolean authenticated = false;

static WebServer OTA_server(80);



void OTAWebUpdater_setup(void) {
  OTA_server.on("/favicon.ico", HTTP_GET,[]() {
      OTA_server.sendHeader("Connection", "close");
      OTA_server.send(200, "text/html", "<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">");
      });

  OTA_server.onNotFound([]() {
      String message = "Server is running!\n\n";
      message += "URI: ";
      message += OTA_server.uri();
      message += "\nMethod: ";
      message += (OTA_server.method() == HTTP_GET) ? "GET" : "POST";
      message += "\nArguments: ";
      message += OTA_server.args();
      message += "\n";
      OTA_server.send(200, "text/plain", message);
      });

  OTA_server.on("/update", HTTP_GET, []() {
  if (!OTA_server.authenticate("MyOwnOTA", s_otaconfig->otapassword))
    //Basic Auth Method with Custom realm and Failure Response
    //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
    //Digest Auth Method with realm="Login Required" and empty Failure Response
    //return server.requestAuthentication(DIGEST_AUTH);
    //Digest Auth Method with Custom realm and empty Failure Response
    //return server.requestAuthentication(DIGEST_AUTH, www_realm);
    //Digest Auth Method with Custom realm and Failure Response
  {
      return OTA_server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
  }
  authenticated = true;
  OTA_server.sendHeader("Connection", "close");
  OTA_server.send(200, "text/html", updateIndex1+s_otaconfig->appname+" "+s_otaconfig->appversion+updateIndex2);
  });

  /*handling uploading firmware file */
  OTA_server.on("/upload", HTTP_POST, []() {
  OTA_server.sendHeader("Connection", "close");
  OTA_server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  delay(3000);
  ESP.restart();
  }, []() {
  HTTPUpload& upload = OTA_server.upload();
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
          OTA_server.close();
          OTA_Wifi_disconnect();
          delay (100);
      } else {
          //Update.printError(Serial);
          debug(Update.errorString());
          //OTAWebUpdaterMessage = Update.errorString();
      }
  }
  });

  OTA_server.begin();
  MDNS.begin(s_otaconfig->device_hostname);
  MDNS.addService("http", "tcp", 80);
  Serial.println("OTA update web server started.");
}
#endif

///////////////////////////////////////////////////////////////////////
// OTA Arduino Update Stuff
///////////////////////////////////////////////////////////////////////
#ifndef DISABLE_ARDUINO_OTA
void ArduinoOTA_setup(void) {
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  ArduinoOTA.setMdnsEnabled(true);
  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(s_otaconfig->device_hostname);
  Serial.printf("Device hostname: %s.local\n", ArduinoOTA.getHostname().c_str());

  // No authentication by default
  //ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  if (s_otaconfig->otapassword != NULL)
    ArduinoOTA.setPassword(s_otaconfig->otapassword);

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
      .onEnd([]() {
          Serial.println("\nEnd");
          OTA_Wifi_disconnect();
          delay (100);
          ArduinoOTA.end();
      })
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

void OTA_setup(OTA_Config_t *otaconfig) {
  s_otaconfig = otaconfig;

  switch(s_otaconfig->otawifitype) {
    case OTA_WIFI_AP:
      OTA_WiFi_AP_setup();
      break;
    case OTA_WIFI_CLIENT:
      OTA_WiFi_Client_setup();
      break;
  }


  switch(s_otaconfig->otatype) {
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

void OTA_handle(void) {
  switch(s_otaconfig->otatype) {
#ifndef DISABLE_ARDUINO_OTA
    case OTA_ARDUINO:
      ArduinoOTA.handle();
      break;
#endif
#ifndef DISABLE_WEB_OTA
    case OTA_WEB:
      OTA_server.handleClient();
      break;
#endif
  }
}
