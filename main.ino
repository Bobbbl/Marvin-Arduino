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

void loop()
{
    String m;
    struct StringArray xm;
    commEnum c = Wait;
    char *token;
    int ks, p;
    char arr[10];
  // Wait for new Message

  while (checkConnection() > 0)
  {

    m = Serial.readStringUntil('@');

    c = GetCommunicationEnum(m);
    m.toCharArray(arr, 20);
    int count = 0;

    switch (c)
    {
    case P:
    token = strtok(arr, ";");
      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }
      p = (int)atoi(xm.str_array[1]);
#if DEBUG_P
      Serial.println(xm.str_array[1]);
#endif
      pump.startMotor(1);
      m = "";
      c = Wait;
      break;
    case S:
    token = strtok(arr, ";");
      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }
      ks = (int)atoi(xm.str_array[1]);

#if DEBUG_S
      Serial.println(xm.str_array[1]);
#endif
      spindel.startMotor(rechts, 1);
      m = "";
      c = Wait;
      break;

    case XYF:
    token = strtok(arr, ";");
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
      stepper_motors.bresenham(s);
#if DEBUG_XYF
      Serial.println(s.x);
      Serial.println(s.y);
      Serial.println(s.f);
#endif
      m = "";
      c = Wait;
      break;


    case NO_VALID_MESSAGE:
      Serial.println("No Valid Message Sent");
      break;

    default:
      Serial.println("Default Case");
      break;
    }
  }

  // Switch Case Which Message Was Received
}