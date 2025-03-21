#include "spiffs_utils.h"

void listFiles(void) {
    Serial.println();
    Serial.println("SPIFFS files found:");

#ifdef ESP32
    // For ESP32, recursively list files using listDir.
    listDir(SPIFFS, "/", true);
#else
    // For ESP8266, open the root directory and iterate through files.
    fs::Dir dir = SPIFFS.openDir("/");
    String line = "=====================================";
    Serial.println(line);
    Serial.println("File name size");
    Serial.println(line);
    while (dir.next()) {
        String fileName = dir.fileName();
        Serial.print(fileName);
        int spaces = 25 - fileName.length();
        if (spaces < 0) spaces = 1;
        while (spaces--) Serial.print(" ");
        fs::File f = dir.openFile("r");
        Serial.print(f.size());
        Serial.println(" bytes");
        yield();
    }
    Serial.println(line);
#endif
    Serial.println();
    delay(1000);
}

#ifdef ESP32
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);
    fs::File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }
    fs::File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR : ");
            String fileName = file.name();
            Serial.println(fileName);
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            String fileName = file.name();
            Serial.print("  " + fileName);
            int spaces = 32 - fileName.length();
            if (spaces < 1) spaces = 1;
            while (spaces--) Serial.print(" ");
            String fileSize = String(file.size());
            spaces = 8 - fileSize.length();
            if (spaces < 1) spaces = 1;
            while (spaces--) Serial.print(" ");
            Serial.println(fileSize + " bytes");
        }
        file = root.openNextFile();
    }
}
#endif
