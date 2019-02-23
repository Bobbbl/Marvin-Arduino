#include "Pin_Defines.h"
#include "Marvin_Communication.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"
#include <string.h>
// #define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>

#define ON 0x01
#define OFF 0x00

#define DEBUG_WHOLEMESSAGE OFF
#define DEBUG_XYF OFF
#define DEBUG_P OFF
#define DEBUG_S OFF
#define ENCODER ON
#define ENCODER_ADVANCED OFF

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
volatile uint8_t pnumberI = 0;

const int chipSelect = CS;

uint8_t endschalter_flag_x = 0, endschalter_flag_y = 0;

// Encoder encoder_spindel(RENC_A, RENC_B);
Marvin_Steppers stepper_motors(PWM1, PWM2, DIR1, DIR2);
Spindel spindel;
PressurePump pump;

ISR(TIMER3_COMPA_vect)
{
  running_flag = true;
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
  // if (pulses_x <= 0)
  // {
  //   pulses_x = 0;
  //   stepper_motors.stopTimer3();
  //   running_flag = false;
  // }
  if (pulses_x <= 0)
  {
    noInterrupts();
    pulses_x = 0;

    if (pnumber > 0)
    {
      Strecke s;
      s.x = nextX[0];
      s.y = nextY[0];
      s.f = nextF[0];
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
      // stepper_motors.bresenham(s);

      //-----------------------------------------------------------------------------------
      static Point lastpoint = {.x = 0, .y = 0};
      Point thispoint;

      Vector v;

      thispoint.x = s.x;
      thispoint.y = s.y;

      v.x = thispoint.x - lastpoint.x;
      v.y = thispoint.y - lastpoint.y;

      float stepsx, stepsy;
      stepsx = abs(v.x * STEPS_PER_MILLIMETER_X);
      stepsy = abs(v.y * STEPS_PER_MILLIMETER_Y);

      unsigned long longline, shortline;

      if (stepsx >= stepsy)
      {
        longline = round(stepsx);
        shortline = round(stepsy);
        longpin = PWM1;
        shortpin = PWM2;
      }
      else
      {
        longline = round(stepsy);
        shortline = round(stepsx);
        longpin = PWM2;
        shortpin = PWM1;
      }

      bcount = (int)round(longline / shortline);

      // Länge Vektor
      float l = sqrt(v.x * v.x + v.y * v.y);

      // Zeit in Sekunden
      float t = l / s.f * 60.0;

      // Zeit pro Schritt
      float st = t / longline;

      // Compare Match Count
      // float count = st * 16000000.00 * this->prescaler;
      int P[5] = {1, 2, 8, 256, 1024};
      long j = 0;
      long P_FRQ = 16000000; // Processor Frequency
      long cc = 1;
      float Feed = s.f; // 100 mm/min
      long PPM = 200;   // 200 Pulses per Millimeter
      unsigned long p = 1;
      long TIMER_MAX = 65536;

      // Zuerst: berechne die Target Frequency
      //
      // Die Target Frequency berechnet sich aus
      // dem Vorschub mit
      // mm/min * Pulses_Per_Millimeter / 60
      float target_frq = (Feed * PPM) / 60.0;
      // Serial.print("Target Frequency: "); Serial.println(target_frq);

      // Suche den richtigen Prescaler
      do
      {
        // Hole den naechsten Prescaler
        // p = P[j];
        if (j == 0)
          p = 1;
        else if (j == 1)
          p = 2;
        else if (j == 2)
          p = 8;
        else if (j == 3)
          p = 256;
        else if (j == 4)
          p = 1024;

        j++;

        // Teste den Prescaler
        // Wenn der Prescaler einen Count generiert der:
        // Einen Count unter 65536 erzeugt
        // Der Count groesser als 0 ist (Gueltige Parameter)
        //
        // dann nimm den Prescaler und trage ihn ein
        cc = P_FRQ / (p * target_frq) - 1;
        // Serial.print("count.... "); Serial.println(cc);
        // Serial.print("prescaler.... "); Serial.println(p);

        if (((cc < TIMER_MAX) && (cc > 1)) || (j > 4))
        {
          break;
        }
      } while (1);

      // Checke Count ob er gültige Werte enthält, wenn nicht
      // dann füge entsprechend entweder den Maximalwert oder
      // den Minimalwert ein
      if (cc <= 0)
        cc = 1;
      else if (cc >= TIMER_MAX)
        cc = TIMER_MAX;

      OCR3A = cc;

      pulses_x = longline;
      pulses_y = shortline;

      // Compare Output Mode
      // Prescaler
      // TCCR3B |= (1 << CS31); // Prescaler 8
      // Interrupts
      // TCCR3B |= (1 << CS32); // Prescaler 256

      switch (p)
      {
      case 1:
        // Set CS12, CS11 and CS10 bits for 1 prescaler
        TCCR3B |= (0 << CS32) | (0 << CS31) | (1 << CS30);
        break;

      case 8:
        // Set CS12, CS11 and CS10 bits for 8 prescaler
        TCCR3B |= (0 << CS32) | (1 << CS31) | (0 << CS30);
        break;

      case 64:
        // Set CS12, CS11 and CS10 bits for 64 prescaler
        TCCR3B |= (0 << CS32) | (1 << CS31) | (1 << CS30);
        break;

      case 256:
        // TCCR3B |= (1 << CS32); // Prescaler 256
        TCCR3B |= (1 << CS32) | (0 << CS31) | (0 << CS30);
        break;

      case 1024:
        TCCR3B |= ((1 << CS30) | (1 << CS32)); // Prescaler 1024
        break;

      default:
        break;
      }
      // Start Timer
      // TCCR3A |= (1 << WGM32); // CTC Mode
      //-----------------------------------------------------------------------------------
      running_flag = true;
    }
    else
    {
      pulses_x = 0;
      stepper_motors.stopTimer3();
      running_flag = false;
    }
    interrupts();
  }
}

int encoder_pin = 2;
float rpm = 0.0;
float velocity = 0;
volatile long pulses = 0;
unsigned long timeold = 0;
unsigned int pulsesperturn = 1;
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
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), counter, RISING);
  pinMode(ENCODER_PIN, INPUT);
}

long positionLeft = -999;

void loop()
{

#if ENCODER_ADVANCED

  long newpos;
  // newpos = encoder_spindel.read();
  // encoder_spindel.write(0);

  // if (millis() - timeold >= 100)
  // {
  //   rpm = (60.0 * 1000.0 / pulsesperturn) / (millis() - timeold) * newpos;
  //   velocity = rpm * 3.1416 * wheel_diameter * 60.0 / 1000000.0;
  //   timeold = millis();
  //   Serial.print(millis() / 1000);
  //   Serial.print("       ");
  //   Serial.print(rpm, DEC);
  //   Serial.print("   ");
  //   Serial.print(newpos, DEC);
  //   Serial.print("     ");
  //   Serial.println(velocity, 2);
  //   encoder_spindel.write(0);
  // }
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
#if DEBUG_XYF
        Serial.println("Point was added to Q");
        Serial.print("X: ");
        Serial.print(nextX[pnumber]);
        Serial.print("Y: ");
        Serial.print(nextY[pnumber]);
        Serial.print("F: ");
        Serial.println(nextF[pnumber]);
        Serial.print("With \"running_flag\" of: ");
        Serial.println(running_flag);
#endif
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
        running_flag = true;
#if DEBUG_XYF
        Serial.println("Motor was started. No points in Q");
        Serial.print("X: ");
        Serial.print(s.x);
        Serial.print("Y: ");
        Serial.print(s.y);
        Serial.print("F: ");
        Serial.println(s.f);
        Serial.print("With \"running_flag\" of: ");
        Serial.println(running_flag);
#endif
      }

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
  // if (digitalRead(ENCODER_PIN) /*&& (micros() - debounce > 500) && digitalRead(ENCODER_PIN)*/)
  // {
    // debounce = micros();
    pulses++;
  // }
  // else
    // ;
}