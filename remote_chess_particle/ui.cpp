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

void upButton() {}
void downButton() {}
void backButton() {}
void nextButton() {}
