#include "Menu.h"
#include "Matrix.h"
#include "Buzzer.h"

void setup() {
  menuSetup();
  buzzerSetup();
  matrixSetup();
}

void loop() {  
  menuLoop();
}