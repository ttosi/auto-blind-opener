#include <Arduino.h>
#include "DRV8825.h"

#define MOTOR_STEPS 200
#define RPM 350
#define STEP 7
#define DIR 8

#define LDR_PIN A6
#define BUTTON_PIN 4

// 17320 degrees = 1 revolution on final output gear
// 3 revolutions to close or open
const long ONE_REV_DEGREES = 17320;
const long NUM_REVS = 4;

uint16_t sensorVoltage = 0;
boolean isDay;
boolean isNight;
boolean hasRunDay = false;
boolean hasRunNight = false;

DRV8825 motor(MOTOR_STEPS, DIR, STEP);

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);

  motor.begin(RPM);
  motor.setMicrostep(8);
}

void loop()
{
  sensorVoltage = analogRead(LDR_PIN);
  Serial.println(sensorVoltage);

  isDay = sensorVoltage > 700 ? true : false;
  isNight = sensorVoltage < 150 ? true : false;

  if (digitalRead(BUTTON_PIN))
  {
    if(isDay)
    {
      hasRunDay = false;
      // Serial.println("buton pushed");
    }
    else
    {
      hasRunNight = false;
    }
  }

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

  delay(1500);
}