#ifndef Globals
#define Globals

// SETTINGS INTERVALS //
#define MIN_LCD_CONTRAST_VALUE 160
#define MAX_LCD_CONTRAST_VALUE 40
#define MAX_LCD_BRIGHTNESS_VALUE 220
#define MIN_LCD_BRIGHTNESS_VALUE 40

// TIMERS //
#define WELCOME_TIME 1000
#define HAVEFUN_TIME 2000
#define FOOD_BLINK 400

// DIRECTIONS //
#define UP 1
#define DOWN -1
#define LEFT -1
#define RIGHT 1
#define STILL 0

// LCD //
#define LCD_WIDTH 16
#define LCD_HEIGTH 2 

// MENU/SUBMENU ITEMS  //
#define START_GAME 1
#define HIGHSCORES 2
#define SETTINGS 3
#define ABOUT 4
#define HOW_TO_PLAY 5

#define YOUR_NAME 1
#define DIFFICULTY 2
#define CONTRAST 3
#define BRIGHTNESS 4
#define MATRIX_LIGHT 5
#define SOUNDS 6
#define RESET_HIGHSCORES 7
#define BACK_TO_MENU 8

// BUTTON & JOYSTICK //
#define BUTTON_PRESSED LOW
const short debounceDelay = 50;
const short minThreshold = 200;
const short maxThreshold = 800;
const short stillMinThreshold = 400;
const short stillMaxThreshold = 600;

// PINS //
const int lcdBrightnessPin = 6;
const int lcdContrastPin = 5;
const int pinSW = 1;
const int pinX = A0;
const int pinY = A1;
const int ledPin = A5;
const int buzzerPin = 3;
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 2;
const byte d6 = 13;
const byte d7 = 4;
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;


// MATRIX
#define MATRIX_SIZE 8
#define MIN_MATRIX_BRIGHTNESS_VALUE 0
#define MAX_MATRIX_BRIGHTNESS_VALUE 15

// SOUNDS

#define MENU_SOUND_TONE 262
#define MENU_SOUND_DURATION 200
#define LOSING_SOUND_TONE 200
#define LOSING_SOUND_DURATION 1000
#define EATING_SOUND_TONE 2093
#define EATING_SOUND_DURATION 100

#endif Globals