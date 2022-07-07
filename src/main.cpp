#include <Arduino.h>
#include <EEPROM.h>
#include "OneButton.h"
#include "DRV8825.h"

#define MOTOR_STEPS 200
#define RPM 350
#define STEP 7
#define DIR 8

#define LDR_PIN A6
#define BUTTON_PIN 2

// 17320 degrees = 1 revolution on final output gear (maybe not)
// 3 revolutions to close or open
const long ONE_REV_DEGREES = 17320;
const long NUM_REVS = 4;
const long TOTAL_REVS = NUM_REVS * ONE_REV_DEGREES;
const long INCREMENT = .05;

uint16_t sensorVoltage = 0;
boolean isDay;
boolean isNight;
boolean hasRunDay = false;
boolean hasRunNight = false;
boolean ledState = false;

DRV8825 motor(MOTOR_STEPS, DIR, STEP);
OneButton button = OneButton(BUTTON_PIN, false, false);

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

void openIncrement()
{
  Serial.println("open at increment");
}

void closeIncrement()
{
  Serial.println("close at increment");
}

void fullOpenClose()
{
  Serial.print("multi click: ");
  Serial.println(button.getNumberClicks());

  switch (button.getNumberClicks())
  {
  case 3:
    Serial.println("full open");
    break;
  case 4:
    Serial.println("full close");
    break;
  default:
    Serial.println("invalid number of clicks");
  }
}

void saveChangesToEeprom()
{
  Serial.println("save changes to eeprom");
  flashLed(10, 100);
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  button.attachClick(openIncrement);
  button.attachDoubleClick(closeIncrement);
  button.attachMultiClick(fullOpenClose);
  button.attachLongPressStop(saveChangesToEeprom);

  motor.begin(RPM);
  motor.setMicrostep(8);

  Serial.println("setup complete");
}

void loop()
{
  sensorVoltage = analogRead(LDR_PIN);
  // Serial.println(sensorVoltage);

  isDay = sensorVoltage > 700 ? true : false;
  isNight = sensorVoltage < 150 ? true : false;

  // open the blinds
  if (isDay && !hasRunDay)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    motor.enable();
    motor.rotate(ONE_REV_DEGREES * 3.52);
    motor.disable();

    digitalWrite(LED_BUILTIN, LOW);

    hasRunDay = true;
    hasRunNight = false;
  }

  // close the blinds
  if (isNight && !hasRunNight)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    motor.enable();
    motor.rotate(-ONE_REV_DEGREES * 3.52);
    motor.disable();

    digitalWrite(LED_BUILTIN, LOW);

    hasRunDay = false;
    hasRunNight = true;
  }

  button.tick();
  delay(50);
}