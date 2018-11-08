#include <Arduino.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);

void setup(){
    Serial.begin(115200);
}

void loop(){
    Serial.println("Test");
}