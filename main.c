#include "SMSlib.h"
#include "assets/title_scene.h"

#define BG_TILES  0
#define GG_W  6
#define GG_H  3
#define GG_AREA 20

#define ST_TITLE 0
#define ST_MAIN 1
#define ST_PAUSE 2
#define ST_GAMEOVER 3
#define START_LIVES 3

unsigned char gameState;
unsigned char lives;
unsigned char score;
unsigned char level;

// GG display 160 x 144
void loadTitleAssets(void) {
  SMS_loadPSGaidencompressedTiles(title_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_W, GG_H, title_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(title_scene__palette__bin);
}

void titleScene(void) {
  unsigned int ks = 0;
  if(gameState != ST_TITLE) {
    return;
  }
  loadTitleAssets();
  SMS_displayOn();
  while(gameState == ST_TITLE) {
    ks = SMS_getKeysStatus();
    if(ks & GG_KEY_START) {
      gameState = ST_MAIN;
      break;
    }
    SMS_waitForVBlank();
    // pause animation
  }
  SMS_displayOff();
}

void main(void) {
  gameState = ST_TITLE;
  lives = START_LIVES;
  score = 0;
  level = 1;
  titleScene();
}
