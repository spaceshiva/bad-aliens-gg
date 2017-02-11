#include <stdbool.h>
#include "SMSlib.h"
#include "assets/assets.h"

#define BG_TILES  0
#define PS_TILES  40

#define GG_X  6
#define GG_Y  3
#define GG_AREA 20

#define ST_TITLE 0
#define ST_MAIN 1
#define ST_GAMEOVER 2
#define START_LIVES 3

unsigned char gameState;
unsigned char lives;
unsigned char score;
unsigned char level;

void mainScene(void) {
  unsigned int ks = 0;
  // load assets
  //SMS_displayOn();
  while(gameState == ST_MAIN) {
    ks = SMS_getKeysStatus();
    SMS_waitForVBlank();
  }
  SMS_displayOff();
  // game over
}

// GG display 160 x 144
void loadTitleAssets(void) {
  SMS_loadPSGaidencompressedTiles(title_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_X, GG_Y, title_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(title_scene__palette__bin);
  SMS_loadPSGaidencompressedTiles(press_start__tiles__psgcompr, PS_TILES);
  GG_setBGPaletteColor(4, RGB(3,3,3));
}

void titleScene(void) {
  unsigned int ks = 0;
  unsigned int x;
  unsigned char s = 1;
  unsigned char t = 1;
  unsigned char d = 1;
  const unsigned int *pnt;
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
    for (x = 0; x < 13; x++) {
      SMS_setNextTileatXY(GG_X+3+x,GG_Y+13);
      if(d == 0) {
        pnt = &press_start__tilemap__bin[x*2];
      } else {
        pnt = &press_start__tilemap__bin[2];
      }
      SMS_setTile(*pnt+++PS_TILES);
    }
    if(t > 60) {
      s++;
      t = 0;
    }
    if(s >= 3) {
      if(d == 1) {
        d = 0;
      } else {
        d = 1;
      }
      s = 0;
    }
    t++;
  }
  SMS_displayOff();
  mainScene();
}

void main(void) {
  gameState = ST_TITLE;
  lives = START_LIVES;
  score = 0;
  level = 1;
  titleScene();
}
