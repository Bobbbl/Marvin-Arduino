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
    // Check Connection to PC and if Interface Program
    // is started
    uint8_t connection_check = checkConnection();
    // Wait until Connection is confirmed
    while(connection_check != 1);

    // Wait for Messages
}