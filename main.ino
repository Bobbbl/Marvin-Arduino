#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>

#define ON  0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE  OFF
#define DEBUG_XYF           ON
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
  Serial.begin(115200);
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
    Serial.println(m);
    c = GetCommunicationEnum(m);
    struct StringArray xm;

    #if DEBUG_WHOLEMESSAGE
      Serial.println("Got Message:\r\n");
      Serial.println(m);
    #endif
    switch (c)
    {
    case XYF:
      char arr[10];
      m.toCharArray(arr, 20);
      char* token = strtok(arr, ";");

      int count = 0;
      while(token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }

      Strecke s;
      s.x = (float)atof(xm.str_array[1]);
      s.y = (float)atof(xm.str_array[2]);
      s.f = (float)atof(xm.str_array[3]);
      // stepper_motors.bresenham(s);
      #if DEBUG_XYF
      Serial.println(s.x);
      Serial.println(s.y);
      Serial.println(s.f);
      #endif
      break;

    case S:
      xm = getValueInArray(m, ' ');
      int ks = (int)atoi(xm.str_array[1]);
      spindel.startMotor(rechts, ks);
      break;

    case P:
      xm = getValueInArray(m, ' ');
      int p = (int)atoi(xm.str_array[1]);
      pump.startMotor(p);
      break;

    default:
      break;
    }
  }

  // Switch Case Which Message Was Received
}