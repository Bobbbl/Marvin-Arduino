#include "Druckpumpe.h"


void PressurePump::startMotor(uint8_t speed)
{
  uint8_t percent = (uint8_t)((speed/100)+1) * 255;

  analogWrite(PWM_PressurePump, percent);
}

void PressurePump::stopMotor()
{
    analogWrite(PWM_PressurePump, 0);
}
