#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "gba.h"
#include "numbers.h"

#include "images/bomb.h"
#include "images/arena.h"
#include "images/small_rupee.h"
#include "images/gameover.h"
#include "images/startscreen.h"
#include "images/link_right.h"
#include "images/win.h"

/* TODO: */
// Add any additional states you need for your app.
typedef enum {
  START,
  PLAY,
  WIN,
  LOSE,
} GBAState;

int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial game state
  GBAState state = START;
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Initial Setup
  Link link = {6, 8, 1};
  const Link reset_link = link;
  Rupee rupee = {randInt(UPPER_B, LOWER_B), randInt(LEFT_B, RIGHT_B)};
  const Rupee reset_rupee = rupee;
  Bomb bomba = {70, 120};
  State currState = {&link, &rupee, &bomba, 0};
  const State reset_state = currState;
  State prevState = currState;

  while (1) {
    switch (state) {
      case START:
        // Resets state to original play state
        link = reset_link;
        rupee = reset_rupee;
        currState = reset_state;
        prevState = currState;
        drawFullScreenImageDMA(startscreen);
        currentButtons = BUTTONS;

        if (KEY_DOWN(BUTTON_START, currentButtons) && !KEY_DOWN(BUTTON_START, previousButtons)) {
          state = PLAY;
        }
        break;
      case PLAY:
        // Initial Screen Drawing
        waitForVBlank();
        drawFullScreenImageDMA(arena);
        //waitForVBlank();
        drawRupee(&rupee);
        //waitForVBlank();
        drawImageDMA(currState.link->row, currState.link->col, LINK_RIGHT_WIDTH, LINK_RIGHT_HEIGHT, link_right);
        //waitForVBlank();
        drawImageDMA(currState.bomba->row, currState.bomba->col, BOMB_WIDTH, BOMB_HEIGHT, bomb);
        //waitForVBlank();
        drawScore(currState.score);

        while(state == PLAY) {
          currentButtons = BUTTONS;

          if (KEY_DOWN(BUTTON_SELECT, currentButtons) && !KEY_DOWN(BUTTON_SELECT, previousButtons)) {
            state = START;
          }

          // Checks if a button is pressed, if so moves in that direction
          if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
            waitForVBlank();
            if (currState.link->col + 10 > 231)
              continue;
            //waitForVBlank();
            (currState.link->col)++;
            currState.link->direction = 1;
            undrawLink(currState.link, 'R');
          } else if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
            if (link.col < 9)
              continue;
            //waitForVBlank();
            (currState.link->col)--;
            currState.link->direction = -1;
            undrawLink(currState.link, 'L');
          } else if (KEY_DOWN(BUTTON_UP, currentButtons)) {
            if (currState.link->row < 7)
              continue;
            (currState.link->row)--;
            //waitForVBlank();
            undrawLink(currState.link, 'U');
          } else if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
            if (currState.link->row + 27 > 130)
              continue;
            (currState.link->row)++;
            //waitForVBlank();
            undrawLink(currState.link, 'D');
          }

          //Checks if Link runs into a rupee
          if (rupeeCollisionDetection(currState.link, currState.rupee)) {
            //waitForVBlank();
            undrawRupee(currState.rupee);
            currState.rupee->row = randInt(UPPER_B, LOWER_B);
            currState.rupee->col = randInt(LEFT_B, RIGHT_B);
            // If rupee spawns on bomb it responds until it doesn't
            while (bombRupeeCollisionDetection(currState.rupee, currState.bomba)) {
              currState.rupee->row = randInt(UPPER_B, LOWER_B);
              currState.rupee->col = randInt(LEFT_B, RIGHT_B);
            }
            //waitForVBlank();
            drawRupee(currState.rupee);
            (currState.score)++;
          }

          // Change to lose state if Link collides with bomb
          if (bombCollisionDetection(currState.link, currState.bomba)) {
            state = LOSE;
          }

          //Update score if score changes
          if (currState.score > prevState.score) {
            //waitForVBlank();
            drawScore(currState.score);
          }

          // Change to win state if score reaches 21
          if (currState.score == 21) {
            state = WIN;
          }
          previousButtons = currentButtons;
          prevState = currState;
        }
        //waitForVBlank();
        break;
      case WIN:
        currentButtons = BUTTONS;
        //waitForVBlank();
        drawFullScreenImageDMA(win);

        //Go to start state if start is pressed
        if (KEY_DOWN(BUTTON_SELECT, currentButtons) && !KEY_DOWN(BUTTON_SELECT, previousButtons)) {
          state = START;
        }
        break;
      case LOSE:
        currentButtons = BUTTONS;
        //waitForVBlank();
        drawFullScreenImageDMA(gameover);

        if (KEY_DOWN(BUTTON_SELECT, currentButtons) && !KEY_DOWN(BUTTON_SELECT, previousButtons)) {
          state = START;
        }
        break;
    }
    previousButtons = currentButtons;  // Store the current state of the buttons
  }
  return 0;
}
