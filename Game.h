#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"
#include "Matrix.h"


struct position {
  byte x, y;
};

position snake[MATRIX_SIZE * MATRIX_SIZE];

unsigned long long lastMoved, foodBlinkTimestamp;

bool sJoyMoved = false, foodDot;
byte snakeX, snakeY ;

byte foodPosX, foodPosY;

short snakeXDirection, snakeYDirection, snakeLength, moveInterval;;

void updateMatrix(byte foodX, byte foodY);
void generateFood();

void gameSetup() {
  clearMatrixLeds();
  moveInterval = 500;
  snakeX = 0;
  snakeY = 2;  
  snakeLength = 3;
  snake[0] = {0, 2};
  snake[1] = {0, 1};
  snake[2] = {0, 0};
  for (byte i = 0; i < snakeLength; i++)
    matrix[snake[i].x][snake[i].y] = 1; 

  generateFood();
  updateMatrix(foodPosX, foodPosY);
  matrixChanged = false;

  lastMoved = 0;
  snakeXDirection = STILL;
  snakeYDirection = RIGHT;
  foodDot = true;
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

}


void blinkFood () {
  unsigned long timePassed = millis() - foodBlinkTimestamp;
  if (timePassed > FOOD_BLINK) {
    foodDot = !foodDot;
    foodBlinkTimestamp = millis();
  }

  if (foodDot) {
    activatePointOnMatrix(foodPosX, foodPosY);
  } else {
    deactivatePointOnMatrix(foodPosX, foodPosY);
  }
}

bool endCondition() {
  if (snakeX == -1 || snakeX == MATRIX_SIZE || snakeY == -1 || snakeY == MATRIX_SIZE)
    return true;
  for (byte i = 1; i < snakeLength; i++) 
    if (snake[i].x == snakeX && snake[i].y == snakeY)
      return true;
  return false;
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

void gameLoop(bool &updateLCD) {

  if (millis() - lastMoved > moveInterval) {
    moveSnake();
  }

  blinkFood();

  getSnakeDirection();

  if (snakeX == foodPosX && snakeY == foodPosY) {
    matrix[foodPosX][foodPosY] = 1;
    matrixChanged = true;
    generateFood();
    snakeLength++;
    moveInterval -= 8;
    updateLCD = true;
  }

  if (matrixChanged == true) {
    updateMatrix(foodPosX, foodPosY);
    matrixChanged = false;
  }
  
}

short getPoints() {
  return snakeLength - 3;
}

#endif Game