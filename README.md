# My Own OTA

This is my everyday wrapper around ArduinoOTA class or the web OTA update.

Both ways of doing OTA software updates are implemented through this small library, you just have to select which one you want to use when initializing the OTA library.

If you are desperately after program size you can remove the unused OTA method at compile time by using the right define : either **DISABLE_ARDUINO_OTA** or **DISABLE_WEB_OTA**.

This can be achieved by adding:
build_flags =
  -DDISABLE_WEB_OTA

in your platformio.ini file.

I have no idea how to achieve this with the Arduino IDE...Please, make yourself a favor and use a decent development environment.
Alternatively those still using the Arduino IDE could add the required #define at the very first line of ota.cpp.
