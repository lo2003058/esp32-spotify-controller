#ifndef WIFICONNECT_H
#define WIFICONNECT_H

#include <Arduino.h>
#include <TFT_eSPI.h>
extern TFT_eSPI tft;
extern TFT_eSprite sprite;

// Initializes Wi-Fi (tries STA mode; if that fails, starts AP mode)
void initWiFi();

// Checks if Wi-Fi is connected and attempts reconnection if needed
bool checkWiFiConnection();

// Draws the current Wi-Fi status on the TFT display
void drawWiFiStatusTFT();

#endif // WIFICONNECT_H
