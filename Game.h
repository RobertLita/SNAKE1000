#include "Arduino.h"
#ifndef Game
#define Game

#include "Globals.h"
#include "StorageManager.h"
#include "Matrix.h"
#include "Buzzer.h"

struct params {
  byte deltaSpeed;
  byte scorePerPoint;
  bool hasWalls;
  bool poisonedFood;
  bool reversedControls;
};

struct position {
  byte x, y;
};

position snake[MATRIX_SIZE * MATRIX_SIZE];
position walls[9], t[3];

const params config[] = {
  { 8, 2, false, false, false },
  { 7, 3, false, true, false },
  { 11, 3, true, false, false },
  { 9, 5, true, true, true },
};

unsigned long long lastMoved, foodBlinkTimestamp;

bool sJoyMoved = false, foodDot, poisoned;
short snakeX, snakeY;

byte foodPosX, foodPosY;

short snakeXDirection, snakeYDirection, snakeLength, moveInterval;


void updateMatrix(byte foodX, byte foodY);
void generateFood();
void generateWalls();
short getPoints();

void gameSetup() {
  pinMode(ledPin, OUTPUT);
  randomSeed(analogRead(0));
  clearMatrixLeds();
  for (byte i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++)
    snake[i] = { 9, 9 };

  if (config[settings.difficulty - 1].hasWalls)
    generateWalls();

  t[0] = walls[0];
  t[1] = walls[1];
  t[2] = walls[2];

  moveInterval = 450;
  snakeX = 0;
  snakeY = 2;
  snakeLength = 3;
  snake[0] = { 0, 2 };
  snake[1] = { 0, 1 };
  snake[2] = { 0, 0 };
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
  if (config[settings.difficulty - 1].poisonedFood) {
    byte chance = random(101);
    if (chance <= 40) {
      digitalWrite(ledPin, HIGH);
      poisoned = true;
    } else {
      digitalWrite(ledPin, LOW);
      poisoned = false;
    }
  } else
    poisoned = false;

  walls[0] = t[0];
  walls[1] = t[1];
  walls[2] = t[2];
  
}


void twistSnake() {
  for (byte i = 0, j = snakeLength - 1; i < j; i++, j--) {
    position aux = snake[i];
    snake[i] = snake[j];
    snake[j] = aux;
  }
  snakeX = snake[0].x;
  snakeY = snake[0].y;

  if (snake[0].x == snake[1].x) {
    if (snake[0].y < snake[1].y) {
      snakeXDirection = STILL;
      snakeYDirection = LEFT;
    } else {
      snakeXDirection = STILL;
      snakeYDirection = RIGHT;
    }
  } else {
    if (snake[0].x < snake[1].x) {
      snakeXDirection = DOWN;
      snakeYDirection = STILL;
    } else {
      snakeXDirection = UP;
      snakeYDirection = STILL;
    }
  }
}

void generateWalls() {
  byte previousRow = 10, previousColumn = 10, startingPointX, startingPointY;
  bool generatedWrong;
  for (byte i = 0; i < 3; i++) {
    byte wallDirection = random(2);
    generatedWrong = true;
    while (generatedWrong) {
      startingPointX = random(1, 5);
      startingPointY = random(1, 5);
      if (abs(int(startingPointX - previousRow)) > 1 && abs(int(startingPointY - previousColumn)) > 1)
        generatedWrong = false;
    }

    for (byte j = 0; j < 3; j++)
      if (wallDirection == 0) {
        matrix[startingPointX][startingPointY + j] = 1;
        walls[i * 3 + j] = { startingPointX, startingPointY + j };
      } else {
        matrix[startingPointX + j][startingPointY] = 1;
        walls[i * 3 + j] = { startingPointX + j, startingPointY };
      }
    previousRow = startingPointX;
    previousColumn = startingPointY;
  }
}


void blinkFood() {
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
  if (snakeX == -1 || snakeX == MATRIX_SIZE || snakeY == -1 || snakeY == MATRIX_SIZE) {
    return true;
  }
  for (byte i = 1; i < snakeLength; i++)
    if (snake[i].x == snakeX && snake[i].y == snakeY) {
      return true;
    }

  if (config[settings.difficulty - 1].hasWalls) {
    for (byte i = 0; i < 9; i++) {
      if (walls[i].x == snakeX && walls[i].y == snakeY) {
        return true;
      }
    }
  }
  return false;
}


void getSnakeDirection() {
  short xValue = analogRead(pinY);
  short yValue = analogRead(pinX);
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

  matrix[snake[snakeLength - 1].x][snake[snakeLength - 1].y] = 0;
  for (byte i = snakeLength - 1; i >= 1; i--)
    snake[i] = snake[i - 1];

  snake[0] = { snakeX, snakeY };
  matrix[snakeX][snakeY] = 1;
  matrixChanged = true;
  lastMoved = millis();
}

byte updateHighscore() {
  short scoreMade = getPoints();
  byte top = 0;
  for (byte i = 0; i < 5; i++)
    if (scoreMade > highscores[i].score) {
      top = i + 1;
      for (byte j = 4; j > i; j--) {
        highscores[j].score = highscores[j - 1].score;
        strcpy(highscores[j].name, highscores[j - 1].name);
      }
      highscores[i].score = scoreMade;
      strcpy(highscores[i].name, settings.name);
      break;
    }
  return top;
}

void gameLoop(bool &updateLCD) {

  if (millis() - lastMoved > moveInterval) {
    moveSnake();
  }

  blinkFood();

  getSnakeDirection();

  if (snakeX == foodPosX && snakeY == foodPosY) {
    if (poisoned) {
      twistSnake();
    }
    walls[0] = t[0];
    walls[1] = t[1];
    walls[2] = t[2];
    eatingSound();
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