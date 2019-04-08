#include "Druckpumpe.h"


void PressurePump::startMotor(int speed, Richtung richtung)
{
  uint8_t percent = (uint8_t)((255.0 / 100.0 * speed));

  if (richtung == rechts)
  {
    digitalWrite(DIR3, HIGH);
    digitalWrite(DIR4, LOW);
  }
  else if(richtung == links)
  {
    digitalWrite(DIR3, LOW);
    digitalWrite(DIR4, HIGH);
  }

  analogWrite(PWM_PressurePump, percent);
}

void PressurePump::stopMotor()
{
  analogWrite(PWM_PressurePump, 0);
}
