#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>

// Fetch a file from the given URL and save it in SPIFFS.
// Returns true if the file was fetched (or updated), false if there was an error.
bool getFile(String url, String filename) {
    // Clear the old file if it exists.
    if (SPIFFS.exists(filename)) {
        SPIFFS.remove(filename);
        Serial.println("Old file cleared: " + filename);
    }

    Serial.println("Downloading " + filename + " from " + url);

    if (WiFiClass::status() == WL_CONNECTED) {
        HTTPClient http; // Create an HTTPClient instance

        // Begin the HTTP connection with the given URL.
        http.begin(url);
        Serial.println("[HTTP] GET...");

        // Send the GET request.
        int httpCode = http.GET();

        if (httpCode > 0) {
            File f = SPIFFS.open(filename, "w+");
            if (!f) {
                Serial.println("File open failed");
                http.end();
                return false;
            }
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if (httpCode == HTTP_CODE_OK) {
                int total = http.getSize();
                int len = total; // total bytes to read
                const size_t buffSize = 512; // Increase buffer size for faster transfer
                uint8_t buff[buffSize];
                WiFiClient *stream = http.getStreamPtr();

                // Set a timeout of 5 seconds for reading data.
                unsigned long startTime = millis();
                while (http.connected() && (len > 0 || len == -1)) {
                    size_t availableSize = stream->available();
                    if (availableSize) {
                        int c = stream->readBytes(buff, (availableSize > buffSize) ? buffSize : availableSize);
                        f.write(buff, c);
                        if (len > 0) {
                            len -= c;
                        }
                        // Reset timeout when data is received.
                        startTime = millis();
                    } else {
                        // If no data for 5 seconds, break out of the loop.
                        if (millis() - startTime > 5000) {
                            Serial.println("Timeout while reading stream");
                            break;
                        }
                        delay(1);
                    }
                }
                Serial.println();
                Serial.println("[HTTP] Connection closed or file end reached.");
            }
            f.close();
        } else {
            Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }

    return true;
}
