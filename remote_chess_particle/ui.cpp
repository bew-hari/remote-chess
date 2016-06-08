#include "globals.h"
#include "ui.h"

void setupButtonInterrupts(){
  pinMode(upPin, INPUT_PULLDOWN);
  attachInterrupt(D7, upButton, RISING);

  pinMode(backPin, INPUT_PULLDOWN);
  attachInterrupt(D6, backButton, RISING);

  pinMode(downPin, INPUT_PULLDOWN);
  attachInterrupt(D5, downButton, RISING);

  pinMode(nextPin, INPUT_PULLDOWN);
  attachInterrupt(D4, nextButton, RISING);
}

// request game
void downButton() {
  // default to AI game
  int gameType = 0;

  board.requestGame(gameType);
}

void backButton() {}

// capture piece
void upButton() {
  board.readCapture();
}

// move piece
void nextButton() {
  board.sendMove();
}
