#ifndef GLOBALS_H
#define GLOBALS_H

#include "board.h"

// global variables
extern Board board;

// pins
extern int latchPin;
extern int clockPin;
extern int dataPin;

extern int upPin;
extern int nextPin;
extern int downPin;

extern int mag1;
extern int mag2;
extern int latch; // latch enable
extern int en;    // driver enable
extern int dir;  // direction
extern int stp;   // step pulse

#endif
