#ifndef SPOTIFYHANDLER_H
#define SPOTIFYHANDLER_H

#include <Arduino.h>
#include <SpotifyEsp32.h>
#include <TFT_eSPI.h>

// Global TFT display instances used for rendering the Spotify info and album art.
extern TFT_eSPI tft;
extern TFT_eSprite sprite;

// Initialize the Spotify client and handle authentication.
// Call this function after initializing SPIFFS and WiFi.
void initSpotify();

// Process the current Spotify information and update the display if changes are detected.
void handleSpotify();

// Render the current playback status (playing or paused) on the TFT.
void drawStatusImage();

// Render the current Spotify information (artist & track name) on the TFT.
void drawSpotifyInfo();

// Declare the global Spotify client instance (defined in spotifyHandler.cpp)
extern Spotify sp;

#endif // SPOTIFYHANDLER_H
