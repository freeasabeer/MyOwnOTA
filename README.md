# My Own OTA

This is a library which provides an easy to use wrapper around ArduinoOTA class or the web OTA update.

Both ways of doing OTA software updates are implemented through this small library, you just have to select which one you want to use when initializing the OTA library.

If you are desperately after program size you can remove the unused OTA method at compile time by using the right define : either **DISABLE_ARDUINO_OTA** or **DISABLE_WEB_OTA**.

This can be achieved by adding:
````
build_flags =
  -DDISABLE_WEB_OTA
````
in your `platformio.ini` file.

I have no idea how to achieve this with the Arduino IDE...Please, make yourself a favor and use a decent development environment.
Alternatively those still using the Arduino IDE could add the required #define at the very first line of ota.cpp.

# Installation instruction

## Arduino IDE
First make sure that all instances of the Arduino IDE are closed. The IDE only scans for libraries at startup. It will not see your new library as long as any instance of the IDE is open!

Download https://github.com/freeasabeer/MyOwnOTA/archive/refs/heads/main.zip and unzip `MyOwnOTA-main.zip`

Rename the unzipped folder `MyOwnOTA-main`to `MyOwnOTA` and move it to the Arduino IDE library folder:
- Windows: C:\Users\<your user name>\Documents\Arduino\Libraries
- Linux $HOME/Arduino/sketchbook/libraries
- MacOS: $HOME/Documents/Arduino/libraries

Restart the Arduino IDE and verify that the library appears in the File->Examples menu

## Platformio
Update the `lib_deps` section of your platformio.ini file as following:
```
lib_deps =
  MyOwnOTA=https://github.com/freeasabeer/MyOwnOTA
```
