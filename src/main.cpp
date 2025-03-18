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

// Define button pin - adjust to match your hardware
#define up 21
#define down 0
#define led 38
bool ledON = false;
bool deb2 = false;

// Button state tracking variables
bool lastButtonState = HIGH; // Assuming pull-up configuration
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce time in ms

// Task handle for button handling thread
TaskHandle_t buttonTaskHandle = NULL;

void handleButtonPress() {
    if (digitalRead(21) == 0) {
        if (deb2 == 0) {
            deb2 = true;
            ledON = !ledON;
            digitalWrite(led, ledON);
        }
    } else deb2 = false;
}

// Button handling task function
void buttonHandlerTask(void *parameter) {
    while (true) {
        handleButtonPress();
        // Small delay to prevent hogging the CPU
        vTaskDelay(10 / portTICK_PERIOD_MS); // 10ms delay
    }
}

void setup() {
    pinMode(up, INPUT_PULLUP);
    pinMode(down, INPUT_PULLUP);
    pinMode(led, OUTPUT);

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

    // Create button handler task on core 0 (app core)
    xTaskCreatePinnedToCore(
        buttonHandlerTask, // Task function
        "ButtonTask", // Task name
        2048, // Stack size (bytes)
        NULL, // Parameters
        2, // Priority (1-24, higher = higher priority)
        &buttonTaskHandle, // Task handle
        0); // Run on Core 0

    delay(1500);

    // Initialize Spotify and perform authentication
    initSpotify();
}

void loop() {
    // No need to handle button presses here anymore
    // It's now handled in its own dedicated task

    static unsigned long lastWiFiCheckTime = 0;
    static unsigned long lastSpotifyCheckTime = 0;
    unsigned long currentTime = millis();

    // Check Wi-Fi every 5 seconds
    if (currentTime - lastWiFiCheckTime >= 5000) {
        checkWiFiConnection();
        lastWiFiCheckTime = currentTime;
    }

    // Handle Spotify every 3 seconds
    if (currentTime - lastSpotifyCheckTime >= 3000) {
        handleSpotify();
        lastSpotifyCheckTime = currentTime;
    }
}
