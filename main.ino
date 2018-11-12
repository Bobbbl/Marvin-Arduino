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
  pinMode(END1, INPUT);
  pinMode(END2, INPUT);
}

void loop(){
  // Enter Starting State
  // Check Connection to PC and if Interface Program
  // is started
  uint8_t connection_check = checkConnection();
  // Wait until Connection is confirmed
  while (connection_check != 1)
    ;

  // Enter Working State
  while (true)
  {
    // Wait for Messages
    communication_alphabet message;
    message = waitForSession();

    // Übertragung gestartet
    if (message == Start_Session)
    {
      message = waitForKonsekutiveMessage();

      switch (message)
      {
      case End_Session:
        // Just end and leeave the if statement
        break;

      case Send_Toolpath:
        /* code */
        break;

      case Start_Homing:
        doHoming();
        break;

      default:
        break;
      }
    }
  }
}