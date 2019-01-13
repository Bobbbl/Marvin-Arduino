#ifndef Druckpumpe
#define Druckpumpe

#include "Arduino.h"
#include "Pin_Defines.h"

class PressurePump
{
    public:
        PressurePump()
        {
            pinMode(PWM_PressurePump, OUTPUT);
            pinMode(IN3, OUTPUT);
            pinMode(IN4, OUTPUT);

            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
            analogWrite(PWM_PressurePump, 0);
        }
    
    void startMotor(uint8_t speed);
    void stopMotor();

};

#endif