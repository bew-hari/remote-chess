#include "globals.h"
#include "sensors.h"

void setupSensors() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);
}

int readReedSwitches(int numSensors) {
  int i;
  int temp = 0;
  int myDataIn = 0;
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially
  digitalWrite(latchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first

  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop

  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's DataPin to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
  for (i=numSensors-1; i>=0; i--)
  {
    digitalWrite(clockPin, 0);
    delayMicroseconds(1);
    temp = digitalRead(dataPin);
    if (temp) {
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    digitalWrite(clockPin, 1);
    delayMicroseconds(1);

  }
  return myDataIn;
}
