#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"
#include "Matrix.h"


struct position {
  byte x, y;
};

position snake[MATRIX_SIZE * MATRIX_SIZE];

short moveInterval = 500;
unsigned long long lastMoved = 0;

bool sJoyMoved = false;
byte snakeX, snakeY = 0;

byte foodPosX, foodPosY;

short snakeXDirection, snakeYDirection, snakeLength;

void updateMatrix();
void generateFood();

void gameSetup() {
  snakeX = 0;
  snakeY = 2;  
  snakeLength = 3;
  snake[0] = {0, 2};
  snake[1] = {0, 1};
  snake[2] = {0, 0};
  for (byte i = 0; i < snakeLength; i++)
    matrix[snake[i].x][snake[i].y] = 1; 

  generateFood();
  updateMatrix();
  matrixChanged = false;

  lastMoved = 0;
  snakeXDirection = STILL;
  snakeYDirection = RIGHT;
}


void generateFood() {
  bool invalidPosition = true;
  while (invalidPosition) {
    foodPosX = rand() % 8;
    foodPosY = rand() % 8;
    for (byte i = 0; i < snakeLength; i++) 
      if (snake[i].x == foodPosX && snake[i].y == foodPosY) {
        invalidPosition = true;
        break;
      }
    else invalidPosition = false;
  }
  
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

  snakeX += snakeXDirection;
  snakeY += snakeYDirection;

  matrix[snake[snakeLength - 1].x][snake[snakeLength - 1].y] = 0;
  for (byte i = snakeLength - 1; i >= 1; i--)
      snake[i] = snake[i - 1]; 

  snake[0] = {snakeX, snakeY};
  matrix[snakeX][snakeY] = 1;
  matrixChanged = true;
  lastMoved = millis();
}

void updateHighscore() {
  for (byte i = 0 ; i < 5; i++)
    if (snakeLength - 3 > highscores[i].score) {
      for (byte j = 4; j > i; j--) {
        highscores[j].score = highscores[j - 1].score;
        strcpy(highscores[j].name, highscores[j - 1].name);
      }
    highscores[i].score = snakeLength - 3;
    strcpy(highscores[i].name, settings.name);
    break;
  }
}

void gameEnded() {
  turnOffMatrix();
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

  if (snakeX == foodPosX && snakeY == foodPosY) {
    generateFood();
    snakeLength++;
    if (snakeLength % 3 == 0) {
      moveInterval -= 20;
    }
    updateLCD = true;
  }
  
}

short getPoints() {
  return snakeLength - 3;
}

#endif Game