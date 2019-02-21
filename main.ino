#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>
#include <Encoder.h>

#define ON 0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE OFF
#define DEBUG_XYF ON
#define DEBUG_P OFF
#define DEBUG_S OFF
#define ENCODER OFF
#define ENCODER_ADVANCED ON

extern volatile int shortpin, longpin;
extern volatile float bcount;
volatile float bcounti = 0;
extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;
extern volatile uint16_t steps_x, steps_y;
extern volatile unsigned long pulses_x, pulses_y;

bool running_flag;
volatile float nextX[100], nextY[100], nextF[100];
volatile uint8_t pnumber = 0;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

Encoder encoder_spindel(RENC_A, RENC_B);
Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);
Spindel spindel;
PressurePump pump;

ISR(TIMER3_COMPA_vect)
{
  running_flag = true;
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
  // if (pulses_x <= 0)
  // {
  //   pulses_x = 0;
  //   stepper_motors.stopTimer3();
  //   running_flag = false;
  // }
  if (pulses_x <= 0)
  {
    pulses_x = 0;

    if (pnumber > 0)
    {
      Strecke s;
      s.x = nextX[pnumber];
      s.y = nextY[pnumber];
      s.f = nextF[pnumber];
      pnumber--;

      if (s.x < 0)
      {
        stepper_motors.setDirectionMotorX((char *)"rechts");
      }
      else
      {
        stepper_motors.setDirectionMotorX((char *)"links");
      }
      if (s.y < 0)
      {
        stepper_motors.setDirectionMotorY((char *)"rechts");
      }
      else
      {
        stepper_motors.setDirectionMotorY((char *)"links");
      }
      stepper_motors.bresenham(s);
      running_flag = true;
    }
    else
    {
      pulses_x = 0;
      stepper_motors.stopTimer3();
      running_flag = false;
    }
  }
}

int encoder_pin = 2;
float rpm = 0.0;
float velocity = 0;
volatile byte pulses = 0;
unsigned long timeold = 0;
unsigned int pulsesperturn = 4096;
const int wheel_diameter = 24;
static volatile unsigned long debounce = 0;

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
  pinMode(RELAY_IN4, OUTPUT);
  stepper_motors.stopTimer3();
  stepper_motors.stopTimer4();
  spindel.setRichtung(keine);
  digitalWrite(RELAY_IN4, HIGH);

  pulses = 0;
  rpm = 0;
  timeold = 0;
  attachInterrupt(0, counter, RISING);
  pinMode(ENCODER_PIN, INPUT);
}

long positionLeft = -999;

void loop()
{

#if ENCODER_ADVANCED

  long newpos;
  newpos = encoder_spindel.read();

  if (millis() - timeold >= 1000)
  {
    rpm = (60.0 * 1000.0 / pulsesperturn) / (millis() - timeold) * newpos;
    velocity = rpm * 3.1416 * wheel_diameter * 60.0 / 1000000.0;
    timeold = millis();
    Serial.print(millis() / 1000);
    Serial.print("       ");
    Serial.print(rpm, DEC);
    Serial.print("   ");
    Serial.print(newpos, DEC);
    Serial.print("     ");
    Serial.println(velocity, 2);
    encoder_spindel.write(0);
  }
#endif

#if ENCODER
  // Encoder
  if (millis() - timeold >= 1000)
  {
    noInterrupts();
    rpm = (60 * 1000 / pulsesperturn) / (millis() - timeold) * pulses;
    velocity = rpm * 3.1416 * wheel_diameter * 60 / 1000000;
    timeold = millis();
    Serial.print(millis() / 1000);
    Serial.print("       ");
    Serial.print(rpm, DEC);
    Serial.print("   ");
    Serial.print(pulses, DEC);
    Serial.print("     ");
    Serial.println(velocity, 2);
    pulses = 0;
    interrupts();
  }
#endif

  // Toolpath Points
  String m;
  struct StringArray xm;
  commEnum c = Wait;
  char *token;
  int ks, p, z;
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
      p = atoi(xm.str_array[1]);
#if DEBUG_P
      Serial.println(p);
#endif
      pump.startMotor(p);
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
      ks = atoi(xm.str_array[1]);

#if DEBUG_S
      Serial.println(ks);
#endif
      spindel.startMotor(rechts, ks);
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

      if (running_flag)
      {
        nextX[pnumber] = (float)atof(xm.str_array[1]);
        nextY[pnumber] = (float)atof(xm.str_array[2]);
        nextF[pnumber] = (float)atof(xm.str_array[3]);
        pnumber++;
      }
      else
      {
        Strecke s;
        s.x = (float)atof(xm.str_array[1]);
        s.y = (float)atof(xm.str_array[2]);
        s.f = (float)atof(xm.str_array[3]);
        if (s.x < 0)
        {
          stepper_motors.setDirectionMotorX((char *)"rechts");
        }
        else
        {
          stepper_motors.setDirectionMotorX((char *)"links");
        }
        if (s.y < 0)
        {
          stepper_motors.setDirectionMotorY((char *)"rechts");
        }
        else
        {
          stepper_motors.setDirectionMotorY((char *)"links");
        }
        stepper_motors.bresenham(s);
      }

#if DEBUG_XYF
      Serial.println(s.x);
      Serial.println(s.y);
      Serial.println(s.f);
#endif
      m = "";
      c = Wait;
      break;

    case Z:
      token = strtok(arr, ";");
      count = 0;
      while (token != NULL)
      {
        strcpy(xm.str_array[count], token);
        count++;
        token = strtok(NULL, ";");
      }
      z = atoi(xm.str_array[1]);
      if (z == 0)
        digitalWrite(RELAY_IN4, HIGH);
      else
        digitalWrite(RELAY_IN4, LOW);
      break;

    case NO_VALID_MESSAGE:
      Serial.println("No Valid Message Sent");
      c = Wait;
      m = "";
      break;

    default:
      Serial.println("Default Case");
      c = Wait;
      m = "";
      break;
    }
  }

  // Switch Case Which Message Was Received
}

void counter()
{
  if (digitalRead(ENCODER_PIN) && (micros() - debounce > 500) && digitalRead(ENCODER_PIN))
  {
    debounce = micros();
    pulses++;
  }
  else
    ;
}