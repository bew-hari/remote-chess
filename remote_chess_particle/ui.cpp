#include "ui.h"

void setupButtonInterrupts(){
  pinMode(D7,INPUT_PULLDOWN);
  attachInterrupt(D7, upButton, RISING);

  pinMode(D6,INPUT_PULLDOWN);
  attachInterrupt(D6, backButton, RISING);

  pinMode(D5,INPUT_PULLDOWN);
  attachInterrupt(D5, downButton, RISING);

  pinMode(D4,INPUT_PULLDOWN);
  attachInterrupt(D4, nextButton, RISING);
}

void upButton() {}
void downButton() {}
void backButton() {}
void nextButton() {}