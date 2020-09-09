#ifndef NUMBERS_H
#define NUMEBRS_H

#define OFFSET(row, col, width) ((col)+(width)*(row))

#define REG_DISPCNT  *(volatile unsigned short *) 0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)

#define SCOREBOARD_COLOR (unsigned short) 0x2127

// The size of the GBA Screen
#define WIDTH 240
#define HEIGHT 160

// This is initialized in gba.c
extern volatile unsigned short *videoBuffer;

void undrawScoreBoard(void);
void drawNumber(int row, int col, char num);
void drawScore(int score);

#endif