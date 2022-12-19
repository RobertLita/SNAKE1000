#include "Arduino.h"
#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"
#include "Matrix.h"

struct params {
  byte deltaSpeed;
  byte scorePerPoint;
  bool hasWalls;
  bool reversedControls; 
};

struct position {
  byte x, y;
};

position snake[MATRIX_SIZE * MATRIX_SIZE];
position walls[9];
const params config[] = { { 8, 1, false, false },
                          { 11, 2, true, false },
                          { 9, 3, true, true },
                        };

unsigned long long lastMoved, foodBlinkTimestamp;

bool sJoyMoved = false, foodDot;
short snakeX, snakeY ;

byte foodPosX, foodPosY;

short snakeXDirection, snakeYDirection, snakeLength, moveInterval;;

void updateMatrix(byte foodX, byte foodY);
void generateFood();
void generateWalls();

void gameSetup() {
  randomSeed(analogRead(0));
  clearMatrixLeds();

  if (config[settings.difficulty - 1].hasWalls)
    generateWalls();
  moveInterval = 450;
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
    invalidPosition = false;
    foodPosX = random(8);
    foodPosY = random(8);
    for (byte i = 0; i < snakeLength; i++) 
      if (snake[i].x == foodPosX && snake[i].y == foodPosY) {
        invalidPosition = true;
        break;
      }
    if (!invalidPosition && config[settings.difficulty - 1].hasWalls)
      for (byte i = 0; i < 9; i++)
        if (walls[i].x == foodPosX && walls[i].y == foodPosY) {
          invalidPosition = true;
          break;
        }
  }

}

void generateWalls() {
  byte previousRow = 10, previousColumn = 10, startingPointX, startingPointY;
  bool generatedWrong;
  for (byte i = 0; i < 3; i++) {
    byte wallDirection = random(2);
    generatedWrong = true;
    while(generatedWrong) {
      startingPointX = random(1, 5);
      startingPointY = random(1, 5);
      Serial.print(startingPointX);
      Serial.print(startingPointY);
      Serial.print('\n');
      if (abs(int(startingPointX - previousRow)) > 1 && abs(int(startingPointY - previousColumn)) > 1) 
        generatedWrong = false;
    }
    
    for (byte j = 0; j < 3; j++)
      if (wallDirection == 0) {
        matrix[startingPointX][startingPointY + j] = 1;
        walls[i * 3 + j] = {startingPointX, startingPointY + j};
      }
      else {
        matrix[startingPointX + j][startingPointY] = 1;
        walls[i * 3 + j] = {startingPointX + j, startingPointY};
      }
    previousRow = startingPointX;
    previousColumn = startingPointY;
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
  if (config[settings.difficulty - 1].hasWalls)
    for (byte i = 0; i < 9; i++) 
      if (walls[i].x == snakeX && walls[i].y == snakeY)
        return true;
  return false;
}


void getSnakeDirection() {
  int xValue = analogRead(pinY);
  int yValue = analogRead(pinX);
  bool reversed = config[settings.difficulty - 1].reversedControls;
  if (stillMinThreshold <= xValue && xValue <= stillMaxThreshold && stillMinThreshold <= yValue && yValue <= stillMaxThreshold) {
    sJoyMoved = false;
  }

  if (xValue > maxThreshold && sJoyMoved == false && snakeXDirection == STILL) {
    if (!reversed)
      snakeXDirection = RIGHT;
    else snakeXDirection = LEFT;
    snakeYDirection = STILL;
    sJoyMoved = true;
  } else if (xValue < minThreshold && sJoyMoved == false && snakeXDirection == STILL) {
    if (!reversed)
      snakeXDirection = LEFT;
    else snakeXDirection = RIGHT;
    snakeYDirection = STILL;
    sJoyMoved = true;
  }

  if (yValue > maxThreshold && sJoyMoved == false && snakeYDirection == STILL) {
    snakeXDirection = STILL;
    if (!reversed)
      snakeYDirection = UP;
    else snakeYDirection = DOWN;
    sJoyMoved = true;
  } else if (yValue < minThreshold && sJoyMoved == false && snakeYDirection == STILL) {
    snakeXDirection = STILL;
    if (!reversed)
      snakeYDirection = DOWN;
    else snakeYDirection = UP;
    sJoyMoved = true;
  }
}

void moveSnake() {

  snakeX += snakeXDirection;
  snakeY += snakeYDirection;
  Serial.print(snake[snakeLength - 1].x);
  Serial.print(snake[snakeLength - 1].y);
  Serial.print('\n');

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
    moveInterval -= config[settings.difficulty - 1].deltaSpeed;
    updateLCD = true;
  }

  if (matrixChanged == true) {
    updateMatrix(foodPosX, foodPosY);
    matrixChanged = false;
  }
  
}

short getPoints() {
  return config[settings.difficulty - 1].scorePerPoint * (snakeLength - 3);
}

#endif Game