#include "spotifyHandler.h"
#include <pins_config.h>  // Should define TFT_WIDTH, TFT_HEIGHT
#include <custom_config.h>  // Should define SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, SPOTIFY_REFRESH_TOKEN, TFT_WIDTH, TFT_HEIGHT

#include "rm67162.h"  // Uncomment if needed

// Instantiate the Spotify client using credentials from pins_config.h
Spotify sp(SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET, SPOTIFY_REFRESH_TOKEN);

// Global variables to store the latest Spotify info
static String lastArtist;
static String lastTrackName;
static String lastAlbumImageUrl;

void initSpotify() {
    sp.begin();
    while (!sp.is_auth()) {
        sp.handle_client();
        Serial.println("Not Authenticated!");
        delay(1000);
    }
    Serial.println("Authenticated");
}

void handleSpotify() {
    String currentArtist = sp.current_artist_names();
    String currentTrackName = sp.current_track_name();
    String currentAlbumImageUrl = sp.get_current_album_image_url(0);

    Serial.println("currentArtist: " + currentArtist);
    Serial.println("currentTrackName: " + currentTrackName);
    Serial.println("currentAlbumImageUrl: " + currentAlbumImageUrl);

    bool updated = false;

    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
        lastArtist = currentArtist;
        Serial.println("Artist: " + lastArtist);
        updated = true;
    }

    if (lastTrackName != currentTrackName && currentTrackName != "Something went wrong" && currentTrackName != "null") {
        lastTrackName = currentTrackName;
        Serial.println("Track: " + lastTrackName);
        updated = true;
    }

    if (updated) {
        drawSpotifyInfo();
        // testDrawSpotify();
    }
}

void testDrawSpotify() {
    sprite.fillSprite(TFT_BLACK);

    sprite.loadFont("Cubic_ecjns_f");
    sprite.drawString("Hello world", 20, 40, 3);
    sprite.drawString("唵嘛呢叭咪吽", 20, 100, 3);
    sprite.drawString("萌え,萌え,キューン", 20, 160, 3);
    sprite.unloadFont();

    lcd_PushColors(0, 0, 536, 240, (uint16_t *) sprite.getPointer());
}


// Draws the current Spotify info onto the TFT display using lcd_PushColors
void drawSpotifyInfo() {
    // Create a sprite the size of the display (TFT_WIDTH x TFT_HEIGHT)
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextSize(2);
    sprite.setTextColor(TFT_WHITE, TFT_BLACK);

    sprite.loadFont("Cubic_ecjns_f");

    // Draw title and Spotify information
    sprite.drawString("Spotify Info", 20, 20);
    sprite.drawString("Artist:", 20, 80);
    sprite.drawString(lastArtist, 95, 80);
    sprite.drawString("Track:", 20, 140);
    sprite.drawString(lastTrackName, 95, 140);

    sprite.unloadFont();

    lcd_PushColors(0, 0, 536, 240, (uint16_t *) sprite.getPointer());
}
