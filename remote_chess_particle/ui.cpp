#include "globals.h"
#include "ui.h"

void setupButtonInterrupts(){
  pinMode(upPin, INPUT_PULLDOWN);
  attachInterrupt(upPin, upButton, RISING);

  pinMode(nextPin, INPUT_PULLDOWN);
  attachInterrupt(nextPin, nextButton, RISING);
}

void upButton() {
  switch(board.state()) {
    case START:
      board.setState(WAIT_FOR_GAME);
      break;

    case WAIT_FOR_GAME:
      break;

    case WAIT_FOR_MOVE:
      break;

    case INVALID_MOVE:
      break;

    case WAIT_FOR_OPP_MOVE:
      break;

    default:
      break;
  }
}

void nextButton() {
  switch(board.state()) {
    case START:
      board.setState(WAIT_FOR_GAME);
      break;

    case WAIT_FOR_GAME:
      break;

    case WAIT_FOR_MOVE:
      break;

    case INVALID_MOVE:
      break;

    case WAIT_FOR_OPP_MOVE:
      break;

    default:
      break;
  }
}
