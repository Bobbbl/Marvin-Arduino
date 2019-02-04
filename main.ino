#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>

#define ON 0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE OFF
#define DEBUG_XYF ON
#define DEBUG_P ON
#define DEBUG_S ON

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
  if (bcount >= bcounti)
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
  Serial.begin(115200);
  while (!Serial)
    pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(END1, INPUT);
  pinMode(END2, INPUT);
  stepper_motors.stopTimer3();
  stepper_motors.stopTimer4();
}

String m;
void loop()
{
  // Wait for new Message

  while (checkConnection() > 0)
  {
    enum commEnum c = Wait;

    m = Serial.readStringUntil('@');

    c = GetCommunicationEnum(m);
    struct StringArray xm;
    char arr[10];
    m.toCharArray(arr, 20);
    char *token = strtok(arr, ";");
    int count = 0;

    switch (c)
    {
    case XYF:

      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }

      Strecke s;
      s.x = (float)atof(xm.str_array[1]);
      s.y = (float)atof(xm.str_array[2]);
      s.f = (float)atof(xm.str_array[3]);
      if(s.x > 0)
        stepper_motors.setDirectionMotorX("rechts");
      else
        stepper_motors.setDirectionMotorX("links");
      if (s.y > 0)
        stepper_motors.setDirectionMotorY("rechts");
      else
        stepper_motors.setDirectionMotorY("links");
      stepper_motors.bresenham(s);
#if DEBUG_XYF
      Serial.println(s.x);
      Serial.println(s.y);
      Serial.println(s.f);
#endif
      break;

    case S:
      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }
      int ks = (int)atoi(xm.str_array[1]);
#if DEBUG_S
      Serial.println(ks);
#endif
      spindel.startMotor(rechts, ks);
      break;

    case P:
      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }
      int p = (int)atoi(xm.str_array[1]);
#if DEBUG_P
      Serial.println(p);
#endif
      pump.startMotor(p);
      break;

    default:
      break;
    }
  }

  // Switch Case Which Message Was Received
}