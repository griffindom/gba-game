#include <stdio.h>

#include "gba.h"
#include "numbers.h"

#include "images/arena.h"
#include "images/numbers/zero.h"
#include "images/numbers/one.h"
#include "images/numbers/two.h"
#include "images/numbers/three.h"
#include "images/numbers/four.h"
#include "images/numbers/five.h"
#include "images/numbers/six.h"
#include "images/numbers/seven.h"
#include "images/numbers/eight.h"
#include "images/numbers/nine.h"


void drawNumber(int row, int col, char num) {
	switch (num) {
		case '0':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, zero);
			break;
		case '1':
			drawImageDMA(row, col, TWO_WIDTH, TWO_HEIGHT, one);
			break;
		case '2':
			drawImageDMA(row, col, THREE_WIDTH, THREE_HEIGHT, two);
			break;
		case '3':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, three);
			break;
		case '4':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, four);
			break;
		case '5':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, five);
			break;
		case '6':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, six);
			break;
		case '7':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, seven);
			break;
		case '8':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, eight);
			break;
		case '9':
			drawImageDMA(row, col, ONE_WIDTH, ONE_HEIGHT, nine);
			break;
	}
}

void undrawScoreBoard(void) {
	for (int r = 0; r < ZERO_HEIGHT; r++) {
    DMA[3].src = arena + ((140 + r) * WIDTH);
    DMA[3].dst = &videoBuffer[OFFSET(140 + r, 17, 240)]; //((col)+(width)*(row + r))
    DMA[3].cnt = 214 | DMA_ON | DMA_SOURCE_FIXED;
  	}
}

void drawScore(int score) {
	char buffer[50];
	sprintf(buffer, "%d", score);
	char* str = buffer;
	int row = 140;
	int col = 17;
	while (*str) {
		//waitForVBlank();
		drawNumber(row, col, *str++);
		col += 10;
	}
}