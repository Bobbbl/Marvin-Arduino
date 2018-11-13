#include <Arduino.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"
#include "Marvin_Communication.h"
#include <SPI.h>
#include <SD.h>

extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);


void setup(){
  Serial.begin(115200);
  while(!Serial); // Wait for Serial to connect
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
  {
    connection_check = checkConnection();
  }
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
        readToolpathTilEnd();
        sendEndSession();
        break;

      case Start_Homing:
        // Do Homing
        stepper_motors.doHoming();
        // Homing done
        sendEndSession();
        break;

      default:
        break;
      }
    }
  }
}