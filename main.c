#include <math.h>
#include "SMSlib.h"
#include "assets/assets.h"

SMS_EMBED_SEGA_ROM_HEADER(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE(1, 0, "chains\\2017", "Bad Aliens GG", "a little concept of a old school shump\nBuilt using devkitSMS & SMSlib");

#define BG_TILES  0
#define PS_TILES  40
#define SCORE_TILES 24
#define SPRITE_TILES 256
#define SHIP_TILE SPRITE_TILES
#define BULLET_TILE (SPRITE_TILES+2)
#define ENEMY_TILE (SPRITE_TILES+4)

#define GG_X  6
#define GG_Y  3
#define GG_AREA 20

#define ST_TITLE 0
#define ST_MAIN 1
#define ST_GAMEOVER 2

#define START_LIVES 3

#define INIT_P_POSX (GG_X * 8) + 80
#define INIT_P_POSY (GG_Y * 8) + 136

unsigned char gameState;
unsigned char playerLives;
unsigned char score;
unsigned char level;
unsigned int playerPosition;

void loadMainAssets(void) {
  SMS_loadPSGaidencompressedTiles(main_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_X, GG_Y, main_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(default__palette__bin);
  SMS_loadPSGaidencompressedTiles(numbers__tiles__psgcompr, SCORE_TILES);
  SMS_loadPSGaidencompressedTiles(sprites__tiles__psgcompr, SPRITE_TILES);
  GG_loadSpritePalette(default__palette__bin);
}

// ref.: http://stackoverflow.com/questions/14564813/how-to-convert-integer-to-character-array-using-c
void numberToArray(char *buf, unsigned long number) {
  unsigned int n = log10f(number) + 1;
  unsigned int i;
  // no dynamic alloc. :(
  // ref.: http://stackoverflow.com/questions/17354905/sdcc-and-malloc-allocating-much-less-memory-than-is-available
  for (i = 0; i < n; ++i, number /= 10) {
    buf[i] = number % 10;
  }
}

void updateScore(unsigned long points) {
  unsigned char x;
  unsigned char pos = 0;
  unsigned char num[5] = {0,0,0,0,0};
  const unsigned int *pnt;
  if(points > 99999) {
    return;
  }
  numberToArray(num, points);
  for(x = 5; x-- > 0 ;){
    SMS_setNextTileatXY(GG_X+7+pos,GG_Y+1);
    pnt = &numbers__tilemap__bin[num[x]*2];
    SMS_setTile(*pnt+++SCORE_TILES);
    pos++;
  }
}

void updateLevel(unsigned char level) {
  const unsigned int *pnt;
  if(level > 10) {
    return;
  }
  SMS_setNextTileatXY(GG_X+14,GG_Y+1);
  pnt = &numbers__tilemap__bin[level*2];
  SMS_setTile(*pnt+++SCORE_TILES);
}

void updateLives(unsigned char lives) {
  const unsigned int *pnt;
  unsigned char x;
  if(lives > 3) {
    return;
  }
  for(x = 0; x < lives; x++) {
    SMS_setNextTileatXY(GG_X+16+x,GG_Y+1);
    pnt = &numbers__tilemap__bin[20];
    SMS_setTile(*pnt+++SCORE_TILES);
  }
}

void movePlayer(void) {
#define MINPLAYERX  (GG_X * 8)
#define MAXPLAYERX  (GG_X * 8) + 152
  unsigned int ks = SMS_getKeysStatus();
  if((ks & PORT_A_KEY_LEFT) && (playerPosition > MINPLAYERX)) {
    playerPosition -= 2;
  } else if((ks & PORT_A_KEY_RIGHT) && (playerPosition < MAXPLAYERX)) {
    playerPosition += 2;
  }
}

void drawPlayer(void) {
  SMS_addSprite(playerPosition, INIT_P_POSY, SHIP_TILE);
}

void mainScene(void) {
  playerPosition = INIT_P_POSX;
  loadMainAssets();
  updateScore(score);
  updateLevel(level);
  updateLives(playerLives);
  SMS_displayOn();
  while(gameState == ST_MAIN) {
    movePlayer();
    SMS_initSprites();
    drawPlayer();
    SMS_finalizeSprites();
    SMS_waitForVBlank();
    SMS_copySpritestoSAT();
  }
  SMS_displayOff();
  // game over
}

// GG display 160 x 144
void loadTitleAssets(void) {
  SMS_loadPSGaidencompressedTiles(title_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_X, GG_Y, title_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(default__palette__bin);
  SMS_loadPSGaidencompressedTiles(press_start__tiles__psgcompr, PS_TILES);
  GG_setBGPaletteColor(4, RGB(3,3,3));
}

void titleScene(void) {
  unsigned int ks = 0;
  unsigned int x;
  const unsigned int *pnt;
  loadTitleAssets();
  SMS_displayOn();
  while(gameState == ST_TITLE) {
    ks = SMS_getKeysStatus();
    if(ks & GG_KEY_START) {
      gameState = ST_MAIN;
      break;
    }
    // press start sentence animation
    for (x = 0; x < 13; x++) {
      SMS_setNextTileatXY(GG_X+3+x,GG_Y+13);
      pnt = &press_start__tilemap__bin[x*2];
      SMS_setTile(*pnt+++PS_TILES);
    }
    SMS_waitForVBlank();
  }
  SMS_displayOff();
  mainScene();
}

void main(void) {
  gameState = ST_TITLE;
  playerLives = START_LIVES;
  score = 0;
  level = 1;
  titleScene();
}
