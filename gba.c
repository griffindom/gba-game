#include <stdio.h> 
#include <stdlib.h>

#include "gba.h"

#include "images/small_rupee.h"
#include "images/bomb.h"
#include "images/arena.h"
#include "images/link_right.h"
#include "images/link_left.h"

volatile unsigned short *videoBuffer = (volatile unsigned short *)0x6000000;
u32 vBlankCounter = 0;

/**
 * Wait for the next draw cycle.
 * Increments vBlankCounter.
 */

void waitForVBlank(void) {
  // TODO: IMPLEMENT
  while(SCANLINECOUNTER > 160)
            ;
  while(SCANLINECOUNTER < 160)
            ;
  vBlankCounter++;
}

/**
 * Draws an image to the entire screen.
 *
 * @param  image pointer to an image
 */

void drawFullScreenImageDMA(const u16 *image) {
  DMA[3].src = image;
  DMA[3].dst = &videoBuffer[0];
  DMA[3].cnt = (HEIGHT * WIDTH) | DMA_ON | DMA_SOURCE_INCREMENT;
} 

/**
 * Draws an image starting at on the screen at [row][col].
 * src is the address of the first element of each row to copy.
 * dst is the address of each line to start copying too. Calculated by adding the
 * offset row from origin to the current row number. Then multiply by width to get to the proper
 * row to copy to. Add param col to get the offset column from origin.
 * cnt will control the amount of data to copy over. Want to copy the width of each row.
 *
 * @param  row starting row of image.
 * @param  col starting col of image.
 * @param  width number of pixels wide
 * @param  height number of pixels tall
 * @param  image pointer to image
 */
void drawImageDMA(int row, int col, int width, int height, const u16 *image) {
  for (int r = 0; r < height; r++) {
    DMA[3].src = image + (r * width);
    DMA[3].dst = &videoBuffer[OFFSET(row + r, col, 240)]; //((col)+(width)*(row + r))
    DMA[3].cnt = width | DMA_ON;
  }
}

/**
 * Draws a rupee starting at [row][col]
 *
 * @param  rupee rupee to draw
 */
void drawRupee(Rupee *rupee) {
  DMA[3].src = small_rupee + 2;
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row, rupee->col + 2, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 1 | DMA_ON;
  DMA[3].src = small_rupee + 6;
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 1, rupee->col + 1, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 3 | DMA_ON;
  for (int r = 2; r < 7; r++) {
    DMA[3].src = small_rupee + (r * 5);
    DMA[3].dst = &videoBuffer[OFFSET(rupee->row + r, rupee->col, 240)]; //((col)+(width)*(row + r))
    DMA[3].cnt = 5 | DMA_ON;
  }
  DMA[3].src = small_rupee + (7 * 5) + 1;
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 7, rupee->col + 1, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 3 | DMA_ON;
  DMA[3].src = small_rupee + (8 * 5);
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 8, rupee->col + 2, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 1 | DMA_ON;
}

/**
 * Redraws a left column of arena onto screen.
 *
 * @param  row row to start draw
 * @param  col col to start draw
 * @param  height height of object moving out of space
 */
void redrawLeftArena(int row, int col, int height) {
  for (int r = 0; r < height; r++) {
    DMA[3].src = arena + (((row + r) * WIDTH) + col - 1);
    DMA[3].dst = &videoBuffer[OFFSET(row + r, col - 1, 240)];
    DMA[3].cnt = 1 | DMA_ON;
  }
}

/**
 * Redraws a right column of arena onto screen.
 *
 * @param  row row to start draw
 * @param  col col to start draw
 * @param  width width of object moving out of space
 * @param  height height of object moving out of space
 */
void redrawRightArena(int row, int col, int width, int height) {
  for (int r = 0; r < height; r++) {
    DMA[3].src = arena + (col + width) + ((row + r) * WIDTH);
    DMA[3].dst = &videoBuffer[OFFSET(row + r, col + width, 240)];
    DMA[3].cnt = 1 | DMA_ON;
  }
}

/**
 * Redraws a bottom row of arena onto screen.
 *
 * @param  row row to start draw
 * @param  col col to start draw
 * @param  height of object moving out of space
 * @param  width width of object moving out of space
 */
void redrawBelowArena(int row, int col, int width, int height) {
  DMA[3].src = arena + (((row + height) * WIDTH) + col);
  DMA[3].dst = &videoBuffer[OFFSET(row + height, col, 240)];
  DMA[3].cnt = width | DMA_ON;
}

/**
 * Redraws a top row of arena onto screen.
 *
 * @param  row row to start draw
 * @param  col col to start draw
 * @param  width width of object moving out of space
 */
void redrawAboveArena(int row, int col, int width) {
  DMA[3].src = arena + (((row - 1) * WIDTH) + col);
  DMA[3].dst = &videoBuffer[OFFSET(row - 1, col, 240)];
  DMA[3].cnt = width | DMA_ON;
}

/**
 * Undraws Link from screen. Redraws the respecitive space he moved out of.
 * Redraws the proper facing link depending on the direction passed in.
 *
 * @param  link Link to draw
 * @param  direction the direction Link moved into
 */
void undrawLink(Link *link, char direction) {
  switch (direction) {
      case 'R':
        waitForVBlank();
        redrawLeftArena(link->row, link->col, LINK_RIGHT_HEIGHT);
        drawImageDMA(link->row, link->col, LINK_RIGHT_WIDTH, LINK_RIGHT_HEIGHT, link_right);
        break;
      case 'L':
        waitForVBlank();
        redrawRightArena(link->row, link->col, LINK_LEFT_WIDTH, LINK_LEFT_HEIGHT);
        drawImageDMA(link->row, link->col, LINK_LEFT_WIDTH, LINK_LEFT_HEIGHT, link_left);
        break;
      case 'U':
        waitForVBlank();
        redrawBelowArena(link->row, link->col, LINK_LEFT_WIDTH, LINK_RIGHT_HEIGHT);
        if (link->direction == 1) {
          drawImageDMA(link->row, link->col, LINK_RIGHT_WIDTH, LINK_RIGHT_HEIGHT, link_right);
        } else {
          drawImageDMA(link->row, link->col, LINK_LEFT_WIDTH, LINK_LEFT_HEIGHT, link_left);
        }
        break;
      case 'D':
        waitForVBlank();
        redrawAboveArena(link->row, link->col, LINK_RIGHT_WIDTH);
        if (link->direction == 1) {
          drawImageDMA(link->row, link->col, LINK_RIGHT_WIDTH, LINK_RIGHT_HEIGHT, link_right);
        } else {
          drawImageDMA(link->row, link->col, LINK_LEFT_WIDTH, LINK_LEFT_HEIGHT, link_left);
        }
        break;
      }
}

/**
 * Undraws a rupee from screen.
 *
 * @param  rupee rupee to undraw
 */
void undrawRupee(Rupee *rupee) {
  DMA[3].src = arena + OFFSET(rupee->row, rupee->col + 2, 240);
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row, rupee->col + 2, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 1 | DMA_ON;
  DMA[3].src = arena + OFFSET(rupee->row + 1, rupee->col + 1, 240);
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 1, rupee->col + 1, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 3 | DMA_ON;
  for (int r = 2; r < 7; r++) {
    DMA[3].src = arena + OFFSET(rupee->row + r, rupee->col, 240);
    DMA[3].dst = &videoBuffer[OFFSET(rupee->row + r, rupee->col, 240)]; //((col)+(width)*(row + r))
    DMA[3].cnt = 5 | DMA_ON;
  }
  DMA[3].src = arena + OFFSET(rupee->row + 7, rupee->col + 1, 240);
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 7, rupee->col + 1, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 3 | DMA_ON;
  DMA[3].src = arena + OFFSET(rupee->row + 8, rupee->col + 2, 240);
  DMA[3].dst = &videoBuffer[OFFSET(rupee->row + 8, rupee->col + 2, 240)]; //((col)+(width)*(row + r))
  DMA[3].cnt = 1 | DMA_ON;
}

/**
 * Detects collision between Link and a rupee.
 *
 * @param  link pointer to Link
 * @param  rupee pointer to rupee
 * @return returns 1 if collision detected, 0 otherwise
 */
int rupeeCollisionDetection(Link *link, Rupee *rupee) {
  if ((link->col < rupee->col + SMALL_RUPEE_WIDTH) &&
    (link->col + LINK_RIGHT_WIDTH > rupee->col) &&
    (link->row < rupee->row + SMALL_RUPEE_HEIGHT) &&
    (link->row + LINK_RIGHT_HEIGHT > rupee->row)) {
      return 1;
  } else {
    return 0;
  }
}

/**
 * Detects collision between Link and a bomb.
 *
 * @param  link pointer to Link
 * @param  bomb pointer to bomb
 * @return returns 1 if collision detected, 0 otherwise
 */
int bombCollisionDetection(Link *link, Bomb *bomb) {
  if ((link->col < bomb->col + BOMB_WIDTH) &&
    (link->col + LINK_RIGHT_WIDTH > bomb->col) &&
    (link->row < bomb->row + BOMB_HEIGHT) &&
    (link->row + LINK_RIGHT_HEIGHT > bomb->row)) {
      return 1;
  } else {
    return 0;
  }
}

/**
 * Detects collision between a bomb and Link.
 *
 * @param  rupee pointer to rupee
 * @param  bomb pointer to bomb
 * @return returns 1 if collision detected, 0 otherwise
 */
int bombRupeeCollisionDetection(Rupee *rupee, Bomb *bomb){
  if ((rupee->col < bomb->col + BOMB_WIDTH) &&
    (rupee->col + SMALL_RUPEE_WIDTH > bomb->col) &&
    (rupee->row < bomb->row + BOMB_HEIGHT) &&
    (rupee->row + SMALL_RUPEE_HEIGHT > bomb->row)) {
      return 1;
  } else {
    return 0;
  }
}

/**
 * Returns a random number between min and max, inclusive.
 *
 * @param  min the left bound of random number
 * @param  max the right bound of a random number
 * @return returns a random integer
 */
int randInt(int min, int max) {
  srand(rand());
  int num = (rand() % (max - min + 1)) + min;
  return num;
}

/**
 * PREWRITTEN FUNCTIONS
 */
static int __qran_seed = 42;
static int qran(void) {
  __qran_seed = 1664525 * __qran_seed + 1013904223;
  return (__qran_seed >> 16) & 0x7FFF;
}

int randint(int min, int max) { return (qran() * (max - min) >> 15) + min; }

void drawChar(int row, int col, char ch, u16 color) {
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 8; j++) {
      if (fontdata_6x8[OFFSET(j, i, 6) + ch * 48]) {
        setPixel(row + j, col + i, color);
      }
    }
  }
}

void drawString(int row, int col, char *str, u16 color) {
  while (*str) {
    drawChar(row, col, *str++, color);
    col += 6;
  }
}

void drawCenteredString(int row, int col, int width, int height, char *str, u16 color) {
  u32 len = 0;
  char *strCpy = str;
  while (*strCpy) {
    len++;
    strCpy++;
  }

  u32 strWidth = 6 * len;
  u32 strHeight = 8;

  int new_row = row + ((height - strHeight) >> 1);
  int new_col = col + ((width - strWidth) >> 1);
  drawString(new_row, new_col, str, color);
}




/**
 * UNUSED FUNCTIONS
 */

void setPixel(int row, int col, u16 color) {
  // TODO: IMPLEMENT
  UNUSED(row);
  UNUSED(col);
  UNUSED(color);
}

void drawRectDMA(int row, int col, int width, int height, volatile u16 color) {
  // TODO: IMPLEMENT
  UNUSED(row);
  UNUSED(col);
  UNUSED(width);
  UNUSED(height);
  UNUSED(color);
}

void fillScreenDMA(volatile u16 color) {
  // TODO: IMPLEMENT
  UNUSED(color);
}
