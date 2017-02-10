#include "SMSlib.h"
#include "assets\initial_scene.h"

#define BG_TILES  0

void loadAssets(void) {
  SMS_loadPSGaidencompressedTiles(initial_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(0, 0, initial_scene__tilemap__stmcompr, 20);
  GG_loadBGPalette(initial_scene__palette__inc);
}

void main(void) {
  loadAssets();
  SMS_displayOn();
  // the game loop
  for(;;) {
    SMS_waitForVBlank();
    /*
    1. Wait for vblank (frame interrupt). The first thing in the loop is a wait for the screen to blank.

'   2. Write to vram from buffers. In order to update vram while the screen is blanked, next thing I do
    is to load the sprite attribute table buffer and the vertical scroll register variable into vram.
    Thise step finishes around line 218, som I'm well within the time frame for vram updating.

    3. Resolve issues rising from current state of game objects. This means collision detection and
    test/responding to counters.

    4. Update game objects. Set new states/positions (x,y), etc. Get, and respond to, user input.
    Calculate new positions for enemies. Note: Nothing happens with regards to sprites in this step.

    5. Update vram buffers. Change the sprites' vertical and horizontal positions.

    When all buffers are updated, we sit back and wait for the next vblank...
    */
  }
}
