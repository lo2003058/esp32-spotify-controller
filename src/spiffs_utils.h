#ifndef SPIFFS_UTILS_H
#define SPIFFS_UTILS_H

#include <Arduino.h>

#ifdef ESP32
  #include "SPIFFS.h"
#else
  #include <FS.h>
  #include <SPIFFS.h>
#endif

// Declare the file listing functions.
void listFiles();

#ifdef ESP32
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
#endif

#endif // SPIFFS_UTILS_H
