#include <math.h>
#include <stdlib.h>
#include "SMSlib.h"
#include "main.h"
#include "assets/assets.h"

SMS_EMBED_SEGA_ROM_HEADER(9999,0);
SMS_EMBED_SDSC_HEADER_AUTO_DATE(1, 0, "chains\\2017", "Bad Aliens GG", "a little concept of a old school shump\nBuilt using devkitSMS & SMSlib");

#define BG_TILES  0
#define PS_TILES  40
#define SCORE_TILES 24
#define SPRITE_TILES 256
#define SHIP_TILE SPRITE_TILES
#define BULLET_TILE (SPRITE_TILES+1)
#define ENEMY_TILE (SPRITE_TILES+2)

#define GG_X  6
#define GG_Y  3
#define GG_AREA 20

#define START_LIVES 3

#define INIT_P_POSX (GG_X * 8) + 80
#define INIT_P_POSY (GG_Y * 8) + 136
#define INIT_E_POSY (GG_Y * 8) + 20

#define MIN_X  (GG_X * 8)
#define MAX_XY  (GG_X * 8) + 152

#define NUMBER_OF_DIGITS 16

unsigned char gameState;
unsigned char playerLives;
unsigned int score;
unsigned char level;
unsigned int playerPosition;
// 0: x, 1: y, 2: dead?
unsigned int bulletBehavior[3];
// 0: X, 1: Y, 2: dead?
unsigned int enemyBehavior[3];

//====================================
// Utils
//====================================

// ref.:http://stackoverflow.com/questions/18691677/how-to-put-an-integer-to-an-array-of-digits
// implementação super simples (mas que funciona)
// retorna o array ao contrário. necessário exibir em ordem reversa
void numberToArray(char *buf, unsigned int number) {
  unsigned int i = 0;
  while(number != 0) {
    buf[i] = number % 10;
    number /= 10;
    i++;
  }
}

//====================================
// Load Assets
//====================================

// GG display 160 x 144
void loadTitleAssets(void) {
  SMS_loadPSGaidencompressedTiles(title_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_X, GG_Y, title_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(default__palette__bin);
  SMS_loadPSGaidencompressedTiles(press_start__tiles__psgcompr, PS_TILES);
  GG_setBGPaletteColor(4, RGB(3,3,3));
}

void loadMainAssets(void) {
  SMS_loadPSGaidencompressedTiles(main_scene__tiles__psgcompr, BG_TILES);
  SMS_loadSTMcompressedTileMapArea(GG_X, GG_Y, main_scene__tilemap__stmcompr, GG_AREA);
  GG_loadBGPalette(default__palette__bin);
  SMS_loadPSGaidencompressedTiles(numbers__tiles__psgcompr, SCORE_TILES);
  SMS_loadPSGaidencompressedTiles(sprites__tiles__psgcompr, SPRITE_TILES);
  GG_loadSpritePalette(default__palette__bin);
}

//====================================
// Variables update
//====================================

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
  for(x = 0; x < 3; x++) {
    SMS_setNextTileatXY(GG_X+16+x,GG_Y+1);
    if(lives >= x + 1) {
      pnt = &numbers__tilemap__bin[20];
    } else {
      pnt = &numbers__tilemap__bin[18];
    }
    SMS_setTile(*pnt+++SCORE_TILES);
  }
}

//====================================
// Behavior
//====================================
int randEnemyXPos(void) {
  unsigned int pos = 0;
  if(playerPosition >= MIN_X && playerPosition <= MIN_X + 16) {
    pos = rand() % MAX_XY;
  } else {
    pos = rand() % playerPosition;
  }
  if(pos < MIN_X) {
    pos = playerPosition;
  }
  if(pos > MAX_XY) {
    pos = MAX_XY;
  }
  return pos;
}

unsigned char checkCollision(unsigned int va[2][2], unsigned int vb[2][2]) {
  if(va[1][0] < vb[0][0] || va[0][0] > vb[1][0]) return 0;
  if(va[1][1] < vb[0][1] || va[0][1] > vb[1][1]) return 0;

  return 1;
}

unsigned char checkBulletEnemyCollision() {
  unsigned int e[2][2];
  unsigned int b[2][2];

  e[0][0] = bulletBehavior[0]+5; //min x
  e[0][1] = bulletBehavior[1]+4;  //min y
  e[1][0] = bulletBehavior[0]+6; //max x
  e[1][1] = bulletBehavior[1]+8; //max y

  b[0][0] = enemyBehavior[0]+2; //min x // +2 pixels para diminuir o box
  b[0][1] = enemyBehavior[1]; // min y
  b[1][0] = enemyBehavior[0]+8; // max x // -1 pixel para diminuir o box
  b[1][1] = enemyBehavior[1]+8; //max y

  return checkCollision(e, b);
}

unsigned char checkEnemyShipCollision() {
  unsigned int e[2][2];
  unsigned int s[2][2];

  s[0][0] = playerPosition; //min x
  s[0][1] = INIT_P_POSY;  //min y (y é fixo)
  s[1][0] = playerPosition+8; //max x
  s[1][1] = INIT_P_POSY+8; //max y

  e[0][0] = enemyBehavior[0]+2; //min x // +2 pixels para diminuir o box
  e[0][1] = enemyBehavior[1]; // min y
  e[1][0] = enemyBehavior[0]+8; // max x
  e[1][1] = enemyBehavior[1]+8; //max y

  return checkCollision(e, s);
}

void checkEnemyOverlapsScreeen(void) {
  if(enemyBehavior[1] >= MAX_XY) {
    enemyBehavior[2] = 1;
  }
}

void checkBulletOverlapsScreen(void) {
  if(bulletBehavior[1] <= (GG_Y * 8) + 16) {
    bulletBehavior[2] = 1;
  }
}

void checkBulletBehavior(void) {
  checkBulletOverlapsScreen();
  if(bulletBehavior[2] == 0) {
    if(checkBulletEnemyCollision() == 1) {
      bulletBehavior[2] = 1;
      enemyBehavior[2] = 1;
      score += 10;
      updateScore(score);
    }
  }
}

void checkEnemyBehavior() {
  checkEnemyOverlapsScreeen();
  if(enemyBehavior[2] == 0) {
    if(checkEnemyShipCollision() == 1) {
      updateLives(--playerLives);
      enemyBehavior[2] = 1;
      playerPosition = INIT_P_POSX;
    }
  }
}

void checkBehavior(void) {
   checkEnemyBehavior();
   checkBulletBehavior();
   if(playerLives == 0) {
     gameState = ST_GAMEOVER;
   }
}
//====================================
// Moving
//====================================
void moveEnemy(void) {
  if(enemyBehavior[2] == 1) {
    enemyBehavior[2] = 0;
    enemyBehavior[1] = INIT_E_POSY;
    enemyBehavior[0] = randEnemyXPos();
  } else {
    ++enemyBehavior[1];
  }
}

void movePlayer(void) {
  unsigned int ks = SMS_getKeysStatus();
  if((ks & PORT_A_KEY_LEFT) && (playerPosition > MIN_X)) {
    playerPosition -= 2;
  } else if((ks & PORT_A_KEY_RIGHT) && (playerPosition < MAX_XY)) {
    playerPosition += 2;
  }
}

void moveBullet(void) {
  unsigned int ks;
  // ok para atirar
  if(bulletBehavior[2] == 1) {
    ks = SMS_getKeysStatus();
    if(ks & PORT_A_KEY_1) {
      bulletBehavior[0] = playerPosition;
      bulletBehavior[1] = INIT_P_POSY + 8;
      bulletBehavior[2] = 0;
    }
  } else {
    --bulletBehavior[1];
  }
}
//====================================
// Drawing
//====================================
void drawPlayer(void) {
  SMS_addSprite(playerPosition, INIT_P_POSY, SHIP_TILE);
}

void drawEnemy(void) {
  if(enemyBehavior[2] == 0) {
      SMS_addSprite(enemyBehavior[0], enemyBehavior[1], ENEMY_TILE);
  }
}

void drawBullet(void) {
  if(bulletBehavior[2] == 0) {
    SMS_addSprite(bulletBehavior[0], bulletBehavior[1], BULLET_TILE);
  }
}

//====================================
// Scenes
//====================================
void gameOverScene(void) {
  gameState = ST_TITLE;
  titleScene();
}

void mainScene(void) {
  playerPosition = INIT_P_POSX;
  enemyBehavior[2] = 1;
  bulletBehavior[2] = 1;
  playerLives = START_LIVES;
  score = 0;
  level = 1;
  loadMainAssets();
  updateScore(score);
  updateLevel(level);
  updateLives(playerLives);
  SMS_displayOn();
  while(gameState == ST_MAIN) {
    movePlayer();
    moveEnemy();
    moveBullet();
    checkBehavior();
    SMS_initSprites();
    drawPlayer();
    drawEnemy();
    drawBullet();
    SMS_finalizeSprites();
    SMS_waitForVBlank();
    SMS_copySpritestoSAT();
  }
  SMS_displayOff();
  gameOverScene();
}

void titleScene(void) {
  unsigned int ks = 0;
  unsigned int x = 0;
  const unsigned int *pnt;
  loadTitleAssets();
  SMS_displayOn();
  while(gameState == ST_TITLE) {
    for (x = 0; x < 13; x++) {
      pnt = &press_start__tilemap__bin[x*2];
      SMS_setNextTileatXY(GG_X+3+x,GG_Y+13);
      SMS_setTile(*pnt+++PS_TILES);
    }
    ks = SMS_getKeysStatus();
    if(ks & GG_KEY_START) {
      gameState = ST_MAIN;
      break;
    }
    SMS_waitForVBlank();
  }
  SMS_displayOff();
  mainScene();
}

void main(void) {
  gameState = ST_TITLE;
  titleScene();
}
