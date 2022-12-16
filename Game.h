#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"
#include "Matrix.h"

const short moveInterval = 500;
unsigned long long lastMoved = 0;

bool sJoyMoved = false;
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

byte foodPosX;
byte foodPosY;

short foodEated, snakeXDirection, snakeYDirection;

void updateMatrix();

void gameSetup() {
  xPos = 0;
  yPos = 0;
  matrix[xPos][yPos] = 1;
  foodPosX = rand() % 8;
  foodPosY = rand() % 8;
  matrix[foodPosX][foodPosY] = 1;
  foodEated = 0;
  updateMatrix();
  lastMoved = 0;
  snakeXDirection = RIGHT;
  snakeYDirection = STILL;
  snakeXDirection = RIGHT;
  snakeYDirection = STILL;
}


void generateFood() {
  foodPosX = rand() % 8;
  foodPosY = rand() % 8;
  matrix[foodPosX][foodPosY] = 1;
  matrixChanged = true;
}


void getSnakeDirection() {
  int xValue = analogRead(pinY);
  int yValue = analogRead(pinX);

  if (stillMinThreshold <= xValue && xValue <= stillMaxThreshold && stillMinThreshold <= yValue && yValue <= stillMaxThreshold) {
    sJoyMoved = false;
  }

  if (xValue > maxThreshold && sJoyMoved == false && snakeXDirection == STILL) {
    snakeXDirection = RIGHT;
    snakeYDirection = STILL;
    sJoyMoved = true;
  } else if (xValue < minThreshold && sJoyMoved == false && snakeXDirection == STILL) {
    snakeXDirection = LEFT;
    snakeYDirection = STILL;
    sJoyMoved = true;
  }

  if (yValue > maxThreshold && sJoyMoved == false && snakeYDirection == STILL) {
    snakeXDirection = STILL;
    snakeYDirection = UP;
    sJoyMoved = true;
  } else if (yValue < minThreshold && sJoyMoved == false && snakeYDirection == STILL) {
    snakeXDirection = STILL;
    snakeYDirection = DOWN;
    sJoyMoved = true;
  }
}

void moveSnake() {
  xLastPos = xPos;
  yLastPos = yPos;
  xPos += snakeXDirection;
  yPos += snakeYDirection;
  matrix[xLastPos][yLastPos] = 0;
  matrix[xPos][yPos] = 1;
  matrixChanged = true;
  lastMoved = millis();
}

void updateHighscore() {
  for (byte i = 0 ; i < 5; i++)
    if (foodEated > highscores[i].score) {
      for (byte j = 4; j > i; j--) {
        highscores[j].score = highscores[j - 1].score;
        strcpy(highscores[j].name, highscores[j - 1].name);
      }
    highscores[i].score = foodEated;
    strcpy(highscores[i].name, settings.name);
    break;
  }
}

void gameEnded() {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      matrix[i][j] = 0;
    }
  }
  updateHighscore();
}

void gameLoop(bool &updateLCD) {

  if (millis() - lastMoved > moveInterval) {
    moveSnake();
  }

  if (matrixChanged == true) {
    updateMatrix();
    matrixChanged = false;
  }

  getSnakeDirection();

  if (xPos == foodPosX && yPos == foodPosY) {
    generateFood();
    foodEated++;
    updateLCD = true;
  }
  
}

short getPoints() {
  return foodEated;
}

#endif Game