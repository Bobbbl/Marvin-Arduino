#include <Arduino.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"
#include "Marvin_Communication.h"

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);


void setup(){
    Serial.begin(115200);
    pinMode(PWM1, OUTPUT);
    pinMode(PWM2, OUTPUT);
    pinMode(DIR1, OUTPUT);
    pinMode(DIR2, OUTPUT);

}

void loop(){
    Serial.println("Test");
}