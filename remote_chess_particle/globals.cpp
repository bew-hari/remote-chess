#include "globals.h"
#include "board.h"

Board board = Board("26003e000447343339373536");

// pins
int latchPin = A3;
int clockPin = A4;
int dataPin = A5;

int upPin = A0;
int nextPin = A1;
int downPin = A2;

// motor stuff
int mag1 = D3;
int mag2 = D2;
int latch = D4; // latch enable
int en = D5;    // driver enable
int dir = D6;  // direction
int stp = D7;   // step pulse
