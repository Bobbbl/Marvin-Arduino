#include "Druckpumpe.h"


void PressurePump::startMotor(int speed)
{
  uint8_t percent = (uint8_t)((255.0/100.0 * speed));

  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, LOW);


  analogWrite(PWM_PressurePump, percent);
}

void PressurePump::stopMotor()
{
    analogWrite(PWM_PressurePump, 0);
}
