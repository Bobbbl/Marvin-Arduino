#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"

extern volatile int shortpin, longpin;
extern volatile float bcount, bcounti = 0;
extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;
extern volatile uint16_t steps_x, steps_y;
extern volatile unsigned long pulses_x, pulses_y;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);

ISR(TIMER3_COMPA_vect)
{
  digitalWrite(longpin, HIGH);
  digitalWrite(longpin, LOW);

  bcounti++;
  if(bcounti >= bcount)
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

void loop()
{
  // Wait for new Message

  while(checkConnection > 0){
    enum commEnum c = Wait;
    String m = Serial.readString();
    String  xMessage, yMessage, 
            sMessage, pMessage;
    struct StringArray strarr = getValueInArray(xMessage, ' ');
    c = GetCommunicationEnum(m);
    
    switch (c)
    {
      case X:
      xMessage = Serial.readString();
      

        break;
    
      case Y:
        break;
    
      case S:
        break;
    
      case P:
        break;
    
      default:
        break;
    }
  }
  

  // Switch Case Which Message Was Received

}