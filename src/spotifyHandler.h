#ifndef SPOTIFYHANDLER_H
#define SPOTIFYHANDLER_H
#include <Arduino.h>
#include <SpotifyEsp32.h>
#include <TFT_eSPI.h>
extern TFT_eSPI tft;
extern TFT_eSprite sprite;

void testDrawSpotify();

// Initialize the Spotify client and handle authentication
void initSpotify();

// Handle current Spotify info and update display if needed
void handleSpotify();

// Draw the current Spotify information (artist & track) on the TFT
void drawSpotifyInfo();

// Declare the global Spotify client instance (defined in the .cpp file)
extern Spotify sp;

#endif // SPOTIFYHANDLER_H
