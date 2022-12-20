#ifndef Buzzer
#define Buzzer

#include "Globals.h"
#include "StorageManager.h"

void buzzerSetup() {
  pinMode(buzzerPin, OUTPUT);
}

void menuBeep() {
  if (!settings.soundsMuted) {
    tone(buzzerPin, MENU_SOUND_TONE, MENU_SOUND_DURATION);
  }
}

void eatingSound() {
  if (!settings.soundsMuted) {
    tone(buzzerPin, EATING_SOUND_TONE, EATING_SOUND_DURATION);
  }
}

void noTone() {
  noTone(buzzerPin);
}

void losingSound() {
  if (!settings.soundsMuted) {
    tone(buzzerPin, LOSING_SOUND_TONE, LOSING_SOUND_DURATION);
  }
}

void winningSound() {
  if (!settings.soundsMuted) {
    tone(buzzerPin, 1700, LOSING_SOUND_DURATION);
  }
}

#endif Buzzer