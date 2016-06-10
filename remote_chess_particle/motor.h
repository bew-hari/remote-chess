/* Function library for stepper motor and electromagnet control
  EECS 347: Autonomous Chessboard */

#include "application.h"

// Function to configure pins for motor control
void setupMotion();

// Generic step helper function, no direction specification
void step();

// Linear motion functions
int stepX(String n);
int stepY(String n);
int squareX(String n);
int squareY(String n);

// 2D motion functions
int move(String mov);
int movePiece(String mov);

// Electromagnet control
int magOn(String n);
int magInv(String n);
int magOff(String n);
