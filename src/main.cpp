#include <Arduino.h>
#include "DRV8825.h"

#define MOTOR_STEPS 200
#define RPM 350
#define STEP 7
#define DIR 8

#define LDR_PIN A6
#define BUTTON_PIN 2

const uint16_t OPEN_BRIGHTNESS = 600;
const uint16_t CLOSE_BRIGHTNESS = 50;

// 17320 degrees = 1 revolution on final output gear (maybe not)
// 3 revolutions to close or open
const long ONE_REV_DEGREES = 17320;
const long NUM_REVS = 3.54;
const long TOTAL_REVS = NUM_REVS * ONE_REV_DEGREES;

uint16_t currentBrightness = 0;

boolean isDay;
boolean isNight;
boolean hasRunDay = false;
boolean hasRunNight = false;
boolean ledState = false;

DRV8825 motor(MOTOR_STEPS, DIR, STEP);

void flashLed(int count, int milliseconds)
{
  for (int i = 0; i < count; i++)
  {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    delay(milliseconds);
  }

  digitalWrite(LED_BUILTIN, LOW);
}

void setup()
{
  // Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  motor.begin(RPM);
  motor.setMicrostep(8);
}

void loop()
{
  currentBrightness = analogRead(LDR_PIN);

  isDay = currentBrightness > OPEN_BRIGHTNESS;
  isNight = currentBrightness < CLOSE_BRIGHTNESS;

  // open the blinds
  if (isDay && !hasRunDay)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    motor.enable();
    motor.rotate(NUM_REVS);
    motor.disable();

    flashLed(10, 100);
    hasRunDay = true;
    hasRunNight = false;
  }

  // close the blinds
  if (isNight && !hasRunNight)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    motor.enable();
    motor.rotate(-NUM_REVS);
    motor.disable();

    flashLed(10, 100);
    hasRunDay = false;
    hasRunNight = true;
  }

  delay(250);
}