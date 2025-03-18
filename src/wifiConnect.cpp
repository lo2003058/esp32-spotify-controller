#include "wifiConnect.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <pins_config.h>
#include "rm67162.h"      // For lcd_PushColors() if using your AMOLED driver

// Create a Preferences instance to store ≈ credentials
Preferences preferences;

// Create an instance of the web server on port 80
AsyncWebServer server(80);

// Variables to hold stored credentials
String storedSSID, storedPassword;

// Function to start AP mode and serve a Wi-Fi configuration portal
void startAP() {
    Serial.println("Starting Access Point (AP) mode...");
    // Use AP+STA mode so the board can serve as an AP and also connect as a station.
    WiFiClass::mode(WIFI_AP_STA);
    // Start softAP with fixed SSID, password, and channel (channel 6)
    bool result = WiFi.softAP("TDisplayS3_Config", "configpassword", 6);
    if (result) {
        Serial.println("SoftAP started successfully.");
        IPAddress apIP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(apIP);
    } else {
        Serial.println("Failed to start SoftAP!");
    }

    // Serve a simple HTML page for Wi-Fi configuration
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>WiFi Setup</title></head><body>";
        html += "<h2>Configure WiFi</h2>";
        html += "<form method='POST' action='/save'>";
        html += "SSID: <input type='text' name='ssid'><br><br>";
        html += "Password: <input type='password' name='password'><br><br>";
        html += "<input type='submit' value='Save'>";
        html += "</form></body></html>";
        request->send(200, "text/html", html);
    });

    // Handle form submission: save new credentials and reboot
    server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
            String newSSID = request->getParam("ssid", true)->value();
            String newPassword = request->getParam("password", true)->value();
            // Save credentials in Preferences
            preferences.putString("ssid", newSSID);
            preferences.putString("password", newPassword);
            request->send(200, "text/html", "<h3>Credentials Saved. Rebooting...</h3>");
            delay(2000);
            ESP.restart();
        } else {
            request->send(400, "text/html", "Missing SSID or Password");
        }
    });

    server.begin();
}

// Helper function to connect to Wi-Fi using stored credentials
void connectToWiFi() {
    // Retrieve stored credentials from Preferences
    storedSSID = preferences.getString("ssid", "");
    storedPassword = preferences.getString("password", "");
    if (storedSSID.length() > 0) {
        Serial.print("Connecting to Wi-Fi: ");
        Serial.println(storedSSID);
        WiFiClass::mode(WIFI_STA);
        WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
        unsigned long startTime = millis();
        // Wait up to 10 seconds for connection
        while (WiFiClass::status() != WL_CONNECTED && millis() - startTime < 10000) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
    }
}

// Public function to initialize Wi-Fi
void initWiFi() {
    Serial.println("Starting WiFi Configuration");
    // Initialize Preferences in namespace "Wi-Fi"
    preferences.begin("wifi", false);

    // Attempt to connect to Wi-Fi using stored credentials
    connectToWiFi();

    // If connection fails, start AP mode (configuration portal)
    if (WiFiClass::status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed. Starting configuration portal...");
        startAP();
    } else {
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    // Draw the current Wi-Fi status on the TFT
    drawWiFiStatusTFT();
}

// Checks if Wi-Fi is connected and attempts reconnection if needed
bool checkWiFiConnection() {
    if (WiFiClass::status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        WiFi.disconnect();
        connectToWiFi();
        drawWiFiStatusTFT();
        return (WiFiClass::status() == WL_CONNECTED);
    }
    return true;
}

// Draws the current Wi-Fi status on the TFT display
void drawWiFiStatusTFT() {
    // Create a sprite the size of the display (adjust resolution as needed)
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);
    sprite.setTextSize(2);
    sprite.loadFont("Cubic_ens");
    if (WiFiClass::status() == WL_CONNECTED) {
        sprite.drawString("SSID: ", 20, 80);
        sprite.drawString(storedSSID, 80, 80);
        sprite.drawString("Local IP: ", 20, 140);
        sprite.drawString(WiFi.localIP().toString(), 110, 140);
    } else {
        sprite.drawString("AP IP: ", 20, 50);
        sprite.drawString(WiFi.softAPIP().toString(), 80, 50);
        sprite.drawString("SSID: TDisplayS3_Config", 20, 110);
        sprite.drawString("Password: configpassword", 20, 170);
    }
    sprite.unloadFont();

    lcd_PushColors(0, 0, 536, 240, (uint16_t *) sprite.getPointer());

    sprite.fillSprite(TFT_BLACK);
}
