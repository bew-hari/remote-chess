/* Function library for stepper motor and electromagnet control
  EECS 347: Autonomous Chessboard */
#include "globals.h"
#include "motor.h"

/* Function to configure particle pins for motor and magnet control */
void setupMotion() {
  // Pin configuration
  pinMode(latch, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(mag1, OUTPUT);
  pinMode(mag2, OUTPUT);

  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(latch, LOW);
  digitalWrite(en, HIGH); // LOW to enable

  digitalWrite(mag1, LOW);
  digitalWrite(mag2, LOW);
}

/* Electromagnet control functions */
int magOn(String cmd) {
  digitalWrite(mag1,LOW);
  digitalWrite(mag2, HIGH);
  return 1;
}

int magInv(String cmd) {
  digitalWrite(mag2,LOW);
  digitalWrite(mag1, HIGH);
  return 1;
}

int magOff(String cmd) {
  digitalWrite(mag1, LOW);
  digitalWrite(mag2, LOW);
  return 1;
}

/* Function to toggle the step pin and trigger one step */
void step() {
  digitalWrite(stp,HIGH);
  delay(1);
  digitalWrite(stp,LOW);
  delay(1);
}

/* Function to move a set number of steps in the X direction */
int stepX(String n) {
  digitalWrite(latch,HIGH); // latch
  // Direction
  if (atof(n)<0) {
    digitalWrite(dir,HIGH);
  } else {
    digitalWrite(dir,LOW);
  };
  digitalWrite(en,LOW); // enable
  // steps
  for (int i=0; i<(abs(atof(n))); i++) {
      step();
  }
  digitalWrite(en,HIGH);
  digitalWrite(latch,LOW);
  return 1;
}

/* Function to move a set number of squares in the X direction
  Input n is a string. A positive input will move the actuator in the
  positive direction, and vice versa. The function can handle decimal inputs
  (ie. to move 0.5 squares) */
int squareX(String n) {
  float num = atof(n);
  // 245 steps per square
  stepX((String)(258*num));
  return 1;
}

/* Function to move a set number of steps in the Y direction */
int stepY(String n) {
  digitalWrite(latch,LOW); // latch
  // Direction
  if (atof(n)<0) {
    digitalWrite(dir,LOW);
  } else {
    digitalWrite(dir,HIGH);
  };
  digitalWrite(en,LOW); // enable
  // steps
  for (int i=0; i<(abs(atof(n))); i++) {
      step();
  }
  digitalWrite(en,HIGH);
  digitalWrite(latch,HIGH);
  return 1;
}

/* Function to move a set number of squares in the Y direction
  Input n is a string. A positive input will move the actuator in the
  positive direction, and vice versa. The function can handle decimal inputs
  (ie. to move 0.5 squares) */
int squareY(String n) {
  float num = atof(n);
  // 245 steps per square
  stepY((String)(258*num));
  return 1;
}

/* Function to move from one square to another
  Input mov is a string of the form 'a2b4' which is interpreted
  as "move from a2 to b4" */
int move(String mov) {
  // Only evaluate move if string is proper length
  if (strlen(mov)==4) {
    // Calculate distance in each direction
    int x = mov[2] - mov[0];
    int y = mov[3] - mov[1];

    // Check for an out-of-bounds move
    if (mov[3]<='8' && mov[3]>='1'
        && mov[1]<='8' && mov[1]>='1'
        && ('i'-mov[0])<=8 && ('i'-mov[0])>=1
        && ('i'-mov[2])<=8 && ('i'-mov[2])>=1) {
      squareX((String)x);
      squareY((String)y);
      return 1;
    }
    else {
      // Move was out of bounds
      Serial.println("Move out of bounds\n");
      return -1;
    }
  }
  else {
    // Input was incorrect
    Serial.println("Incorrect input length\n");
    return -1;
  }
}

/* Function to move a piece
  Function assumes the electromagnet begins at position A1
  Input mov is a string of the form 'a2b4' which is interpreted
  as "move piece from a2 to b4" */
int movePiece(String mov) {
  // Move magnet to piece in question
  move(String("a1"+mov.substring(0,2)));
  delay(500);
  // Turn on magnet
  magOn("x");
  delay(500);
  // Move piece onto gridlines
  squareX("-0.5");
  delay(500);
  squareY("-0.5");
  delay(500);
  // Move piece
  move(mov);
  delay(500);
  // Move piece to center of square
  squareX("0.5");
  squareY("0.5");
  delay(500);
  // Turn off magnet
  magOff("x");
  delay(500);
  // Move back to A0 to await next command
  move(String(mov.substring(2,4)+"a1"));
  return 1;
}
