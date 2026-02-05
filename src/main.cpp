#include "shuman.h"
#include <EEPROM.h>

// =======================
// DEFINITIONS
// =======================
#define PIN_BUTTON 4
#define PIN_LED_FREQ_1 5
#define PIN_LED_FREQ_2 6

#define EEPROM_BUTTONSTEP_ADDR 0x00

// =======================
// VARIABLES
// =======================
uint8_t buttonStep = 1;
uint8_t fadeInLedStep = 0;
uint8_t ledFreq=1;
bool enableFadeIn=true;

// =======================
// EEPROM HELPERS
// =======================
void saveButtonStepToEEPROM()
{
  EEPROM.update(EEPROM_BUTTONSTEP_ADDR, buttonStep);
}

uint8_t loadButtonStepFromEEPROM()
{
  return EEPROM.read(EEPROM_BUTTONSTEP_ADDR);
}

// =======================
// LED FADE IN
// =======================
void showLedFadeIn()
{
  switch (ledFreq)
  {
    case 1:
      if (fadeInLedStep == 0)
      {
        digitalWrite(PIN_LED_FREQ_2, LOW);
        setOutputFrequency(FREQ_1);
      }
      fadeInLedStep++;
      if (fadeInLedStep <255)
      {
        analogWrite(PIN_LED_FREQ_1, fadeInLedStep);
        delay(15);
      }
      else
      {
        fadeInLedStep = 0;
        ledFreq = 2;
      }
      break;

    case 2:
      if (fadeInLedStep == 0)
      {
        digitalWrite(PIN_LED_FREQ_1, LOW);
        setOutputFrequency(FREQ_2);
      }
      fadeInLedStep++;
      if (fadeInLedStep <255)
      {
        analogWrite(PIN_LED_FREQ_2, fadeInLedStep);
        delay(15);
      }
      else
      {
        fadeInLedStep = 0;
        ledFreq = 1;
      }
      break;
  }
}

// =======================
// LED FLASH + EEPROM
// =======================
void showLedFlash(float FREQ)
{
  enableFadeIn=false;
  digitalWrite(PIN_LED_FREQ_1, LOW);
  digitalWrite(PIN_LED_FREQ_2, LOW);

  for (size_t i = 0; i < 3; i++)
  {
    digitalWrite((FREQ == FREQ_1 ? PIN_LED_FREQ_1 : PIN_LED_FREQ_2), HIGH);
    delay(500);
    digitalWrite((FREQ == FREQ_1 ? PIN_LED_FREQ_1 : PIN_LED_FREQ_2), LOW);
    delay(500);
  }
}

// =======================
// STEP HANDLING
// =======================
void manageStep(uint8_t step)
{
    switch (step)
    {
      case 1:
        fadeInLedStep = 0;
        enableFadeIn=true;
        saveButtonStepToEEPROM();
        break;

      case 2:
        setOutputFrequency(FREQ_1);
        saveButtonStepToEEPROM();
        showLedFlash(FREQ_1);
        break;

      case 3:
        setOutputFrequency(FREQ_2);
        saveButtonStepToEEPROM();
        showLedFlash(FREQ_2);
        break;
    }
}

// =======================
// BUTTON HANDLING
// =======================
void manageFreqButton()
{
  int buttonState = digitalRead(PIN_BUTTON);
  if (buttonState == HIGH)
  {
    buttonStep++;
    manageStep(buttonStep);
    if (buttonStep >= 3)
      buttonStep = 0;
    saveButtonStepToEEPROM();
    delay(500); // Debounce
  }
}

// =======================
// SETUP
// =======================
void setup()
{
  initSineTable();
  initPWM();

  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED_FREQ_1, OUTPUT);
  pinMode(PIN_LED_FREQ_2, OUTPUT);

  // Load the saved button step (1,2,3)
  uint8_t storedButtonStep = loadButtonStepFromEEPROM();
  if (storedButtonStep >= 1 && storedButtonStep <= 3)
  {
    buttonStep = storedButtonStep;
    manageStep(buttonStep);
  }
}

// =======================
// LOOP
// =======================
void loop()
{
  manageFreqButton();
  if (enableFadeIn)
    showLedFadeIn();
}
