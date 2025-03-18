#include <Arduino.h>
#include <TFT_eSPI.h>
#include "rm67162.h"

//customer function
#include "wifiConnect.h"
#include "spotifyHandler.h"
#include "spiffs_utils.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

byte brightness[7] = {100, 120, 140, 180, 200, 230, 254};

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS initialisation failed!");
        while (1) yield(); // Stay here twiddling thumbs waiting
    }
    Serial.println("\r\nInitialisation done.");

    listFiles(); // Lists the files so you can see what is in the SPIFFS

    rm67162_init(); // amoled lcd initialization
    lcd_setRotation(3);
    sprite.createSprite(536, 240);
    sprite.setSwapBytes(true);

    lcd_brightness(brightness[3]);

    // Connect to Wi-Fi using the function from wifiConnect.cpp
    initWiFi();

    delay(1500);

    // Initialize Spotify and perform authentication
    initSpotify();
}

void loop() {
    static unsigned long lastCheckTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime >= 5000) {
        checkWiFiConnection();
        lastCheckTime = currentTime;
    }
    if (currentTime - lastCheckTime >= 1000) {
        handleSpotify();
        lastCheckTime = currentTime;
    }
}
