# ESP32 Spotify Display

A Spotify now-playing display for the LilyGo T-Display S3 that shows currently playing track information on the built-in
AMOLED display.

## Features

- Wi-Fi connectivity with automatic configuration portal
- Spotify integration showing current artist and track
- Custom font support

## Hardware Requirements

- LilyGo T-Display S3 (ESP32-S3 with AMOLED display)
- USB cable for programming

## Software Requirements

- PlatformIO IDE
- Required libraries (automatically installed via platformio.ini):
    - ESPAsyncWebServer
    - ArduinoJson
    - AsyncTCP
    - base64
    - TFT_eSPI
    - SpotifyEsp32

## Setup

1. **Clone the repository**

2. **Create configuration file**:
    - Copy `src/__example_custom_config.h__` to `src/custom_config.h`
    - Add your Spotify API credentials

3. **Create data directory**:
   ```bash
   mkdir data
   ```

4. **Add font files to data directory** (if using custom fonts):
    - Place your font files in the `data` directory

5. **Upload filesystem**:
   ```bash
   pio run --target uploadfs
   ```

6. **Compile and upload**:
   ```bash
   pio run --target upload
   ```

## Configuration

### Spotify API Setup

1. Create a Spotify Developer account at https://developer.spotify.com
2. Create a new application to get your Client ID and Secret
3. Generate a refresh token using Spotify's authorization flow
4. Update these values in `custom_config.h`

### WiFi Configuration

On first boot, the device creates an access point named "TDisplayS3_Config" with password "configpassword". Connect to
this network and browse to the displayed IP address to configure your WiFi credentials.

## Usage

After setup, the device will:

1. Connect to your WiFi network
2. Authenticate with Spotify
3. Display currently playing track information

The display updates automatically when a new song plays.

## Troubleshooting

- If SPIFFS doesn't initialize, ensure you've created a data directory and uploaded the filesystem
- For WiFi connection issues, check the serial monitor for debugging information
- If Spotify authentication fails, verify your API credentials in `custom_config.h`