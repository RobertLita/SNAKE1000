#ifndef Matrix
#define Matrix

#include "LedControl.h"
#include "LCDSymbols.h"
#include "Globals.h"

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
bool matrixChanged = true;

byte matrix[][MATRIX_SIZE] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

void matrixSetup() {
  lc.shutdown(0, false);
  lc.clearDisplay(0);
}

void setMatrixBrightness(byte value) {
  lc.setIntensity(0, value);
}


void updateMatrix() {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      lc.setLed(0, i, j, matrix[i][j]);
    }
  }
}

void displayIconOnMatrix(const byte icon[]) {
  lc.clearDisplay(0);
  for (byte row = 0; row <= MATRIX_SIZE; row++)
    lc.setRow(0, row, icon[row]);
}


void turnOffMatrix() {
  lc.clearDisplay(0);
}


#endif Matrix