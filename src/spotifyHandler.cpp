#include "spotifyHandler.h"
#include "rm67162.h"  // For LCD display functions
#include "assetsImport.h"
#include <custom_config.h>  // Should define SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, SPOTIFY_REFRESH_TOKEN, TFT_WIDTH, TFT_HEIGHT


// Global sub-sprite for text drawing (size: 286×240)
TFT_eSprite textSprite = TFT_eSprite(&tft); // assuming your main display is "tft"

// Instantiate the Spotify client using credentials
Spotify sp(SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, SPOTIFY_REFRESH_TOKEN);

// Global variables to store the latest Spotify info
static String lastArtist;
static String lastTrackName;
static bool lastStatus;

const int availableWidth = 286; // Text area width (x=0 to 286)
const int availableHeight = 240; // Text area height (y=0 to 240)

void initSpotify() {
    sp.begin();
    while (!sp.is_auth()) {
        sp.handle_client();
        Serial.println("Not Authenticated!");
        delay(1000);
    }
    Serial.println("Authenticated");
    // Create the text sub-sprite with the defined size
    textSprite.createSprite(availableWidth, availableHeight);
}

void handleSpotify() {
    // Get the latest Spotify info
    String currentArtist = sp.current_artist_names();
    String currentTrackName = sp.current_track_name();
    bool currentStatus = sp.is_playing();

    bool updated = false;

    // Update artist if changed
    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
        lastArtist = currentArtist;
        Serial.println("Artist: " + lastArtist);
        updated = true;
    }
    // Update track if changed
    if (lastTrackName != currentTrackName && currentTrackName != "Something went wrong" && currentTrackName != "null") {
        lastTrackName = currentTrackName;
        Serial.println("Track: " + lastTrackName);
        updated = true;
    }
    // Update playing status if changed
    if (lastStatus != currentStatus) {
        lastStatus = currentStatus;
        Serial.println("Status: " + String(lastStatus ? "Playing" : "Paused"));
        updated = true;
    }

    if (updated) {
        drawSpotifyInfo();
        drawStatusImage();
    }

    // Push the main sprite to the display (assumes main sprite covers entire display)
    lcd_PushColors(0, 0, 536, 240, (uint16_t *) sprite.getPointer());
}

void drawStatusImage() {
    // Draw the play/pause image in the right area of the main sprite
    if (sp.is_playing()) {
        sprite.pushImage(296, 0, music_play_240Width, music_play_240Height, music_play_240);
    } else {
        sprite.pushImage(296, 0, music_pause_240Width, music_pause_240Height, music_pause_240);
    }
}

void drawSpotifyInfo() {
    // Clear and set up the text sub-sprite
    textSprite.fillSprite(TFT_BLACK);
    textSprite.setTextColor(TFT_WHITE, TFT_BLACK);
    textSprite.loadFont("Cubic_ecjns_f");
    textSprite.setTextSize(2);

    // Calculate maximum width available for text (with margin)
    const int maxWidth = availableWidth - 40; // 20px margin on each side

    // Handle artist text truncation
    String displayArtist = lastArtist;
    int artistWidth = textSprite.textWidth(displayArtist);
    if (artistWidth > maxWidth) {
        // Calculate width of ellipsis
        int ellipsisWidth = textSprite.textWidth("...");

        // Truncate text until it fits with ellipsis
        while (artistWidth > maxWidth - ellipsisWidth && displayArtist.length() > 0) {
            displayArtist = displayArtist.substring(0, displayArtist.length() - 1);
            artistWidth = textSprite.textWidth(displayArtist);
        }
        displayArtist += "...";
    }

    // Handle track name truncation
    String displayTrack = lastTrackName;
    int trackWidth = textSprite.textWidth(displayTrack);
    if (trackWidth > maxWidth) {
        int ellipsisWidth = textSprite.textWidth("...");

        while (trackWidth > maxWidth - ellipsisWidth && displayTrack.length() > 0) {
            displayTrack = displayTrack.substring(0, displayTrack.length() - 1);
            trackWidth = textSprite.textWidth(displayTrack);
        }
        displayTrack += "...";
    }

    // Draw the truncated text
    textSprite.drawString(displayArtist, 20, 80);
    textSprite.drawString(displayTrack, 20, 140);

    textSprite.unloadFont();

    // Copy text sprite to main sprite
    sprite.pushImage(0, 0, availableWidth, availableHeight, (uint16_t *) textSprite.getPointer());
}
