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
      board.changeState(WAIT_FOR_GAME);
      break;

    case WAIT_FOR_MOVE:
      board.changeState(READ_MOVE);
      break;

    case GAME_OVER:
      board.changeState(START);
      break;

    default:
      break;
  }
}

static State savedState;
void nextButton() {

  savedState = board.state();

  if (board.state() != DEBUG_SENSORS) {
    board.changeState(DEBUG_SENSORS);
  } else {
    board.changeState(savedState);
  }

  /*
  switch(board.state()) {
    case START:
      board.changeState(WAIT_FOR_GAME);
      break;

    case WAIT_FOR_MOVE:
      board.changeState(READ_MOVE);
      break;

    default:
      break;
  }
  */
}
