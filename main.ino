#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>

#define ON  0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE  OFF
#define DEBUG_XYF           OFF
#define DEBUG_P             OFF
#define DEBUG_S             OFF

extern volatile int shortpin, longpin;
extern volatile float bcount, bcounti = 0;
extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;
extern volatile uint16_t steps_x, steps_y;
extern volatile unsigned long pulses_x, pulses_y;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);
Spindel spindel;
PressurePump pump;

ISR(TIMER3_COMPA_vect)
{
  digitalWrite(longpin, HIGH);
  digitalWrite(longpin, LOW);

  bcounti++;
  if (bcounti >= bcount)
  {
    bcounti = 0;
    digitalWrite(shortpin, HIGH);
    digitalWrite(shortpin, LOW);
  }

  pulses_x--;
  if (pulses_x <= 0)
  {
    pulses_x = 0;
    stepper_motors.stopTimer3();
  }
}

void setup()
{
  Serial.begin(1000000);
  while (!Serial)
    ; // Wait for Serial to connect
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

  while (checkConnection() > 0)
  {
    enum commEnum c = Wait;
    String m = Serial.readString();
    String xMessage, yMessage,
        sMessage, pMessage;
    c = GetCommunicationEnum(m);
    struct StringArray xm;

    #if DEBUG_WHOLEMESSAGE
      Serial.println("Got Message:\r\n");
      Serial.println(m);
      Serial.println("commEnum\n");
      switch (c)
      {
        case Wait:
          Serial.println("Wait");
          break;
        case XYF:
          Serial.println("XYF");
          break;
      
        case S:
          Serial.println("S");
          break;

        case P:
          Serial.println("P");
          break;
        default:
          break;
      }
    #endif
    switch (c)
    {
    case XYF:
      //xm = getValueInArray(m, ' ');
      char arr[10];
      m.toCharArray(arr, 10);
      char* token = strtok(arr, " ");
      while(token != NULL)
      {
        Serial.println(token);
        Serial.println("-----");
        token = strtok(NULL, " ");
      }

      #if DEBUG_XYF
      Serial.println("XYF was entered:");
      
      for(int i = 0; i < 10; i++)
      {
        Serial.println(xm.str_array[i]);
      }
      Serial.println("-------------------");
      
      #endif
      Strecke s;
      s.x = xm.str_array[1].toFloat();
      s.y = xm.str_array[2].toFloat();
      s.f = xm.str_array[3].toFloat();
      stepper_motors.bresenham(s);
      break;

    case S:
      xm = getValueInArray(m, ' ');
      int ks = xm.str_array[1].toInt();
      spindel.startMotor(rechts, ks);
      break;

    case P:
      xm = getValueInArray(m, ' ');
      int p = xm.str_array[1].toInt();
      pump.startMotor(p);
      break;

    default:
      break;
    }
  }

  // Switch Case Which Message Was Received
}