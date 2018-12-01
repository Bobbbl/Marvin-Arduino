#include <Arduino.h>
#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"

extern volatile int shortpin, longpin;
extern volatile unsigned long bcount, bcounti = 0;
extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;
extern volatile uint16_t steps_x, steps_y;
extern volatile unsigned long pulses_x, pulses_y;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);

// This is Motor X
// ISR(TIMER3_COMPA_vect)
// {
//   pulses_x--;
//   if(pulses_x <= 0)
//   {
//     // Stop Timer 3
//     stepper_motors.stopTimer3();
//     stepper_motors.stopTimer4();
//     pulses_x = 0;
//   }
//   if(pulses_x > 0)
//   {
//   digitalWrite(PWM1, !digitalRead(PWM1));
//   TCNT3 = 0;
//   }
//   TCNT3 = 0;
// }

// ISR(TIMER4_COMPA_vect)
// {
//   digitalWrite(PWM2, !digitalRead(PWM2));
// }

ISR(TIMER3_COMPA_vect)
{
  digitalWrite(longpin, HIGH);
  digitalWrite(longpin, LOW);

  bcounti++;
  if(bcounti == bcount)
  {
    bcounti = 0;
    digitalWrite(shortpin, HIGH);
    digitalWrite(shortpin, LOW);
  }
  
  pulses_x--;
  if(pulses_x <= 0)
  {
    pulses_x = 0;
    stepper_motors.stopTimer3();
  }

}

void setup(){
  Serial.begin(115200);
  while(!Serial); // Wait for Serial to connect
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(END1, INPUT);
  pinMode(END2, INPUT);
  stepper_motors.stopTimer3();
  stepper_motors.stopTimer4();


}

void loop(){
  // Enter Starting State
  // Check Connection to PC and if Interface Program
  // is started
  uint8_t connection_check = checkConnection();
  Strecke s;
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
    message = No_Message;
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
        // Wait for Message
        while(1){
          // Receive new Point - Function handles all Communication
          s = receivePoint();

          // drive Motor
          if (s.error == 0 && s.end_session == 0)
          {
            // Strecke_Steps_RPM s1 = convertToStepsAndRPM(s);
            // stepper_motors.stepPWM(s1);
            // stepper_motors.easyStep(s);
            // stepper_motors.tt(s);
            stepper_motors.bresenham(s);

            while(steps_x != 0 || steps_y != 0){
            }
            sendPointReached();
          }
          // In other case leave while loop
          else
          {
            break;
          }
        }

        break;

      case Start_Homing:
        // Do Homing
        Serial.println("Homing");
        stepper_motors.doHoming();
        // Homing done
        sendEndSession();
        break;

      default:
        sendEndSession();
        break;
      }
    }
  }
}