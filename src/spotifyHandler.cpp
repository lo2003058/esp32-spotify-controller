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

// Define drawing limits and scrolling parameters
const int availableWidth = 286; // Text area width (x=0 to 286)
const int availableHeight = 240; // Text area height (y=0 to 240)
const int scrollDelay = 100; // Delay in milliseconds between scroll updates
const int spacing = 20; // Gap between successive copies for smooth scrolling

// Separate timers and offsets for artist and track scrolling
unsigned long lastScrollTimeArtist = 0;
unsigned long lastScrollTimeTrack = 0;
int artistScrollOffset = 0;
int trackScrollOffset = 0;

/**
 * drawScrollingText - Draws a single line of scrolling text like a news ticker.
 *
 * @param spr            The sprite to draw into.
 * @param text           The text to draw.
 * @param posY           The vertical position to draw the text.
 * @param availableWidth The maximum width allowed for the text.
 * @param scrollOffset   Reference to the current scroll offset (updated by the function).
 * @param lastScrollTime Reference to the last scroll update time.
 */
void drawScrollingText(TFT_eSprite &spr, const String &text, int posY, int availableWidth, int &scrollOffset,
                       unsigned long &lastScrollTime) {
    int16_t textWidth = spr.textWidth(text);
    if (textWidth > availableWidth) {
        unsigned long now = millis();
        if (now - lastScrollTime > scrollDelay) {
            scrollOffset++;
            if (scrollOffset > textWidth + spacing) {
                scrollOffset = 0;
            }
            lastScrollTime = now;
        }
        int posX = 0 - scrollOffset;
        spr.drawString(text, posX, posY);
        // Draw a second copy for seamless scrolling
        if (posX + textWidth < availableWidth) {
            spr.drawString(text, posX + textWidth + spacing, posY);
        }
    } else {
        // If the text fits, draw it statically
        spr.drawString(text, 0, posY);
    }
}

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

/**
 * handleSpotify - Retrieves new Spotify data, updates global variables,
 * resets ticker offsets if text changes, and (if needed) updates the status image.
 * Call this function periodically (e.g. every few seconds) to fetch new data.
 */
void handleSpotify() {
    // Get the latest Spotify info
    String currentArtist = sp.current_artist_names();
    String currentTrackName = sp.current_track_name();
    bool currentStatus = sp.is_playing();

    bool updatedText = false;
    bool updatedStatus = false;

    // Update artist if changed
    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
        lastArtist = currentArtist;
        Serial.println("Artist: " + lastArtist);
        updatedText = true;
        artistScrollOffset = 0; // Reset ticker offset when new text arrives
    }
    // Update track if changed
    if (lastTrackName != currentTrackName && currentTrackName != "Something went wrong" && currentTrackName != "null") {
        lastTrackName = currentTrackName;
        Serial.println("Track: " + lastTrackName);
        updatedText = true;
        trackScrollOffset = 0; // Reset ticker offset when new text arrives
    }
    // Update playing status if changed
    if (lastStatus != currentStatus) {
        lastStatus = currentStatus;
        Serial.println("Status: " + String(lastStatus ? "Playing" : "Paused"));
        updatedStatus = true;
    }

    // Always update the scrolling text (even if text didn't change)
    drawSpotifyInfo();

    // Update the status image only when needed
    if (updatedStatus) {
        drawStatusImage();
    }

    // Push the main sprite to the display (assumes main sprite covers entire display)
    lcd_PushColors(0, 0, 536, 240, (uint16_t *) sprite.getPointer());
}

/**
 * updateSpotifyTicker - Continuously updates the scrolling ticker.
 * Call this function frequently (e.g., in your main loop) so that the text scrolls smoothly.
 */
void updateSpotifyTicker() {
    drawSpotifyInfo();
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
    textSprite.setTextSize(2);
    textSprite.setTextColor(TFT_WHITE, TFT_BLACK);
    textSprite.loadFont("Cubic_ecjns_f");

    // Draw scrolling text for the artist and track names at specified positions
    drawScrollingText(textSprite, lastArtist, 80, availableWidth, artistScrollOffset, lastScrollTimeArtist);
    drawScrollingText(textSprite, lastTrackName, 140, availableWidth, trackScrollOffset, lastScrollTimeTrack);

    textSprite.unloadFont();

    // Copy the text area into the main sprite at (0,0)
    sprite.pushImage(0, 0, availableWidth, availableHeight, (uint16_t *) textSprite.getPointer());
}
