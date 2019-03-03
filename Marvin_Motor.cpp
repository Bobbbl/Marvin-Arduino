#include "Marvin_Motor.h"
#include <Arduino.h>

volatile int longpin, shortpin;
volatile float bcount = 0;
volatile uint16_t steps_x = 0, steps_y = 0;
volatile unsigned long pulses_x = 0, pulses_y = 0;
extern Marvin_Steppers stepper_motors;

void Marvin_Steppers::doHoming()
{
  // Set Speed to pretty Slow
  // 60 RPMs = 1 RPS = 200 Steps per Second
  this->setSpeed_x(60);
  this->setSpeed_y(60);

  checkLimitSwitches();
  // Homing X
  // Drive in X Direction until I hit Limit Switch
  while (endschalter_flag_x != 0)
  {
    checkLimitSwitches();
    this->step(-10, 0);
  }
  // After hitting Limit Switch, drive Back 10 Steps
  this->step(10, 0);

  checkLimitSwitches();
  // Homing Y
  // Drive in Y Direction until I hit Limit Switch
  while (endschalter_flag_y != 0)
  {
    checkLimitSwitches();
    this->step(0, -10);
  }
  // After hitting Limit Switch, drive Back 10 Steps
  this->step(0, 10);
}

void Marvin_Steppers::checkLimitSwitches()
{
  endschalter_flag_x = digitalRead(END1);
  endschalter_flag_y = digitalRead(END2);
}

void Marvin_Steppers::stepMotorX(Richtung r)
{
  // check limit switches
  checkLimitSwitches();
  // Check if end switch is toggled
  if (endschalter_flag_x == 0)
  {
    return;
  }

  if (r == links)
  {
    digitalWrite(DIR1, HIGH);
  }
  else
  {
    digitalWrite(DIR1, LOW);
  }

  if (endschalter_flag_x == 1)
  {
    digitalWrite(this->pwm_x, !digitalRead(this->pwm_x));
  }
}

void Marvin_Steppers::stepMotorY(Richtung r)
{
  // Check limit switches
  checkLimitSwitches();
  // Check if end switch is toggled
  if (endschalter_flag_y == 0)
  {
    return;
  }

  if (r == links)
  {
    digitalWrite(DIR2, HIGH);
  }
  else
  {
    digitalWrite(DIR2, LOW);
  }

  if (endschalter_flag_y == 1)
  {
    digitalWrite(this->pwm_y, !digitalRead(this->pwm_y));
  }
}

void Marvin_Steppers::setSpeed_x(long whatSpeed)
{
  this->step_delay_x = 60L * 1000L * 1000L / this->number_of_steps_x / whatSpeed;
}

void Marvin_Steppers::setSpeed_y(long whatSpeed)
{
  this->step_delay_y = 60L * 1000L * 1000L / this->number_of_steps_y / whatSpeed;
}

void Marvin_Steppers::stepPWM(Strecke_Steps_RPM s)
{
  // Set speed X
  // Formel: MAXCOUNT - Compare Match Time*CPUFRQ/Prescaler
  float spm = 65536 - s.rpm_x * this->number_of_steps_x;
  float ips = 2 * spm;
  float cmt = 1 / ips;
  float count = 65536 - cmt * 16000000L / this->prescaler;
  uint16_t round_count = (uint16_t)round(count);
  OCR3A = round_count * 10;
  // Set speed Y
  spm = 65536 - s.rpm_y * this->number_of_steps_y;
  ips = 2 * spm;
  cmt = 1 / ips;
  count = 65536 - cmt * 16000000L / this->prescaler;
  round_count = (uint16_t)round(count);
  OCR4A = round_count * 10;
  Serial.println(round_count);
  // Set Steps
  if (s.steps_x < 0)
  {
    this->setDirectionMotorX((char *)"rechts");
    steps_x = -s.steps_x;
  }
  else
  {
    this->setDirectionMotorX((char *)"links");
    steps_x = s.steps_y;
  }

  if (s.steps_y < 0)
  {
    this->setDirectionMotorY((char *)"rechts");
    steps_y = -s.steps_y;
  }
  else
  {
    this->setDirectionMotorY((char *)"links");
    steps_y = s.steps_y;
  }

  this->startTimer3(64);
  this->startTimer4(64);
}

void Marvin_Steppers::easyStep(Strecke s)
{
  static Point lastpoint = {.x = 0, .y = 0};
  Point thispoint;

  Vector v;

  thispoint.x = s.x;
  thispoint.y = s.y;

  v.x = thispoint.x - lastpoint.x;
  v.y = thispoint.y - lastpoint.y;

  float f = s.f;
  Serial.println("Vorschub");
  Serial.println(f);
  float winkel = acos((v.x * 1 + v.y * 0) / (sqrt(v.x * v.x + v.y * v.y) * sqrt(1 * 1 + 0 * 0)));

  float fx = cos(winkel) * f;
  float fy = sin(winkel) * f;

  float tx = v.x / fx;
  float ty = v.y / fy;
  uint16_t prescaler = 8;

  // float countx = abs((tx * 60 * CPU_FREQ) / (STEPS_PER_MILLIMETER_X * v.x * 2 * this->prescaler));
  // float county = abs((ty * 60 * CPU_FREQ) / (STEPS_PER_MILLIMETER_Y * v.y * 2 * this->prescaler));
  float countx = (1800.00 * 16000000.00 * sqrt(v.x * v.x + v.y * v.y)) / (STEPS_PER_MILLIMETER_X * prescaler * f * v.x);
  float county = (1800.00 * 16000000.00 * sqrt(v.x * v.x + v.y * v.y)) / (STEPS_PER_MILLIMETER_Y * prescaler * f * v.y);

  float rcountx = round(countx);
  float rcounty = round(county);

  OCR3A = rcountx;
  OCR4A = rcounty;

  unsigned long pulsesx, pulsesy;
  pulsesx = round(STEPS_PER_MILLIMETER_X * v.x * 2);
  pulsesy = round(STEPS_PER_MILLIMETER_Y * v.y * 2);

  // Set Steps
  if (v.x < 0)
  {
    this->setDirectionMotorX((char *)"rechts");
    pulses_x = pulsesx;
  }
  else
  {
    this->setDirectionMotorX((char *)"links");
    pulses_x = pulsesx;
  }

  if (v.y < 0)
  {
    this->setDirectionMotorY((char *)"rechts");
    pulses_y = pulsesy;
  }
  else
  {
    this->setDirectionMotorY((char *)"links");
    pulses_y = pulsesy;
  }

  lastpoint.x = thispoint.x;
  lastpoint.y = thispoint.y;

  this->startTimer3(prescaler);
  this->startTimer4(prescaler);
}

void Marvin_Steppers::bresenham(Strecke s)
{
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

  TCCR3A = 0;
  TCCR3B = 0;
  TCNT3 = 0;

  Point lastpoint = {.x = 0, .y = 0};
  Point thispoint;

  Vector v;

  thispoint.x = s.x;
  thispoint.y = s.y;

  v.x = s.x;
  v.y = s.y;

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

  // Compare Match Count
  int P[6] = {1, 2, 8, 64, 256, 1024};
  long j = 0;
  long P_FRQ = 16000000; // Processor Frequency
  long cc = 1;
  float Feed = s.f; // 100 mm/min
  long PPM = 400;   // 200 Pulses per Millimeter
  unsigned long p = 1;
  long TIMER_MAX = 65536;

  // Zuerst: berechne die Target Frequency
  //
  // Die Target Frequency berechnet sich aus
  // dem Vorschub mit
  // mm/min * Pulses_Per_Millimeter / 60
  float target_frq = ((Feed * PPM) / 60.0) * 2.0;

  // Suche den richtigen Prescaler
  do
  {
    // Hole den naechsten Prescaler
    // p = P[j];
    if (j == 0)
      p = 1;
    else if (j == 1)
      p = 8;
    else if (j == 2)
      p = 64;
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

  // Den Compare - Wert setzen
  OCR3A = cc;

  pulses_x = longline;
  pulses_y = shortline;

  // Timer starten

  DDRE |= (1 << 3);
  TCCR3A |= (1 << WGM32);  // CTC Mode
  TIMSK3 |= (1 << OCIE3A); // Output Compare Interrupt Enabled
  TCCR3B = 0;
  if (p == 1)
    TCCR3B |= (1 << CS30);
  else if (p == 8)
    TCCR3B |= (1 << CS31);
  else if (p == 64)
    TCCR3B |= (0 << CS32) | (1 << CS31) | (1 << CS30);
  else if (p == 256)
    TCCR3B |= (1 << CS32) | (0 << CS31) | (0 << CS30);
  else if (p == 1024)
    TCCR3B |= ((1 << CS30) | (1 << CS32)); 
}

void Marvin_Steppers::tt(Strecke s)
{

  // this->prescaler = 256;

  static Point lastpoint = {.x = 0, .y = 0};
  Point thispoint;

  Vector v;

  thispoint.x = s.x;
  thispoint.y = s.y;

  v.x = thispoint.x - lastpoint.x;
  v.y = thispoint.y - lastpoint.y;

  float pulsesx, pulsesy;
  pulsesx = abs(v.x * STEPS_PER_MILLIMETER_X);
  pulsesy = abs(v.y * STEPS_PER_MILLIMETER_Y);

  float l = sqrt(v.x * v.x + v.y * v.y);

  // Zeit in Sekunden
  float t = l / s.f * 60.0;

  // Zeit pro Schritt
  float stx = t / pulsesx;
  float sty = t / pulsesy;

  // Frequenzen
  float hzx = 1 / stx;
  float hzy = 1 / sty;
  OCR3A = 65000;

  // Zeit in ms
  t = t * 1000;
  this->startTimer3(256);
}

void Marvin_Steppers::step(int steps_to_move_x, int steps_to_move_y)
{
  int steps_left_x = abs(steps_to_move_x); // how many steps to take

  int steps_left_y = abs(steps_to_move_y); // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move_x > 0)
  {
    this->direction_x = 1;
  }
  if (steps_to_move_x < 0)
  {
    this->direction_x = 0;
  }

  if (steps_to_move_y > 0)
  {
    this->direction_y = 1;
  }
  if (steps_to_move_y < 0)
  {
    this->direction_y = 0;
  }

  // decrement the number of steps, moving one step each time:
  while (steps_left_x > 0 || steps_left_y > 0)
  {
    unsigned long now = micros();
    // move only if the appropriate delay has passed:
    if (now - this->last_step_time_x >= this->step_delay_x)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time_x = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction_x == 1)
      {
        this->step_number_x++;
        if (this->step_number_x == this->number_of_steps_x)
        {
          this->step_number_x = 0;
        }
      }
      else
      {
        if (this->step_number_x == 0)
        {
          this->step_number_x = this->number_of_steps_x;
        }
        this->step_number_x--;
      }
      // decrement the steps left:
      steps_left_x--;
    }

    if (now - this->last_step_time_y >= this->step_delay_y)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time_y = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction_y == 1)
      {
        this->step_number_y++;
        if (this->step_number_y == this->number_of_steps_y)
        {
          this->step_number_y = 0;
        }
      }
      else
      {
        if (this->step_number_y == 0)
        {
          this->step_number_y = this->number_of_steps_y;
        }
        this->step_number_y--;
      }
      // decrement the steps left:
      steps_left_y--;
    }
  }
}

void Marvin_Steppers::stopTimer3()
{
  // Stop Timer
  // TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
  TCCR3B = 0;
  TCCR3A = 0;
  // TCCR3B = 0x00;
}

void Marvin_Steppers::startTimer3(unsigned long prescaler)
{
  DDRE |= (1 << 3);
  // Start Timer
  TCCR3A |= (1 << WGM32); // CTC Mode
  // Compare Output Mode
  // TCCR3A |= (1 << COM3A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
  // TCCR3A &= ~(1 << COM3A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
  // Prescaler
  // this->prescaler = prescaler;
  // TCCR3B |= (1 << CS31); // Prescaler 8
  // Interrupts
  // TCCR3B |= (1 << CS32); // Prescaler 256

  switch (prescaler)
  {
  case 1:
    // Set CS12, CS11 and CS10 bits for 1 prescaler
    TCCR3B |= (0 << CS32) | (0 << CS31) | (1 << CS30);
    this->prescaler = 1;
    break;

  case 8:
    // Set CS12, CS11 and CS10 bits for 8 prescaler
    TCCR3B |= (0 << CS32) | (1 << CS31) | (0 << CS30);
    this->prescaler = 8;
    break;

  case 64:
    // Set CS12, CS11 and CS10 bits for 64 prescaler
    TCCR3B |= (0 << CS32) | (1 << CS31) | (1 << CS30);
    this->prescaler = 64;
    break;

  case 256:
    // TCCR3B |= (1 << CS32); // Prescaler 256
    TCCR3B |= (1 << CS32) | (0 << CS31) | (0 << CS30);
    this->prescaler = 256;
    break;

  case 1024:
    TCCR3B |= ((1 << CS30) | (1 << CS32)); // Prescaler 1024
    this->prescaler = 1024;
    break;

  default:
    break;
  }

  TIMSK3 |= (1 << OCIE3A); // Output Compare Interrupt Enabled
}

void Marvin_Steppers::stopTimer4()
{
  // Stop Timer
  // cli();
  TCCR4B &= ~((1 << CS42) | (1 << CS41) | (1 << CS40));
  // sei();
}

void Marvin_Steppers::startTimer4(unsigned long prescaler)
{
  // Start Timer
  // TCCR4B |= (1 << CS42); // Prescaler 256
  switch (prescaler)
  {
  case 1:
    TCCR4B |= (1 << CS40); // Prescaler 1
    this->prescaler = 1;
    break;

  case 8:
    TCCR4B |= (1 << CS41); // Prescaler 8
    this->prescaler = 8;
    break;

  case 64:
    TCCR4B |= ((1 << CS40) | (1 << CS41)); // Prescaler 64
    this->prescaler = 64;
    break;

  case 256:
    TCCR4B |= (1 << CS42); // Prescaler 256
    this->prescaler = 256;
    break;

  case 1024:
    TCCR4B |= ((1 << CS40) | (1 << CS42)); // Prescaler 1024
    this->prescaler = 1024;
    break;

  default:
    break;
  }
}

void Marvin_Steppers::setDirectionMotorX(char *str)
{
  if (strcmp("rechts", str) == 0)
  {
    digitalWrite(DIR1, HIGH);
  }
  else if (strcmp("links", str) == 0)
  {
    digitalWrite(DIR1, LOW);
  }
}

void Marvin_Steppers::setDirectionMotorY(char *str)
{
  if (strcmp("rechts", str) == 0)
  {
    digitalWrite(DIR2, HIGH);
  }
  else if (strcmp("links", str) == 0)
  {
    digitalWrite(DIR2, LOW);
  }
}

Strecke_Steps_RPM convertToStepsAndRPM(Strecke s)
{
  float distance_per_step_x = DISTANCE_PER_REVOLUTION_X / STEPS_PER_REVOLUTION_X;
  float distance_per_step_y = DISTANCE_PER_REVOLUTION_Y / STEPS_PER_REVOLUTION_Y;

  Strecke_Steps_RPM ssp;
  ssp.steps_x = (int)s.x / distance_per_step_x;
  ssp.steps_y = (int)s.y / distance_per_step_y;

  /* RPM */
  static Point last_point = {.x = 0, .y = 0};
  Point this_point;
  Vector vector;

  this_point.x = s.x;
  this_point.y = s.y;
  // Vector berechnen
  vector.x = this_point.x - last_point.x;
  vector.y = this_point.y - last_point.y;
  // Laenge Vektor berechnen
  float h = sqrtf(vector.x * vector.x + vector.y * vector.y);

  // Zeit berechnen
  float t = h / s.f;
  t = t * 60;
  // Millimeter in Steps
  unsigned long sx = abs(s.x) * STEPS_PER_MILLIMETER_X;
  unsigned long sy = abs(s.y) * STEPS_PER_MILLIMETER_Y;
  // Umdrehungen
  float rx = sx / STEPS_PER_REVOLUTION_X;
  float ry = sy / STEPS_PER_REVOLUTION_Y;
  // RPMs
  float rpmx = rx / t * 60;
  float rpmy = ry / t * 60;

  ssp.rpm_x = rpmx;
  ssp.rpm_y = rpmy;
  ssp.error = 0;
  ssp.end_session = 0;

  last_point.x = this_point.x;
  last_point.y = this_point.y;

  return ssp;
}

void Spindel::setRichtung(Richtung richtung)
{
  if (richtung == links)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  else if (richtung == rechts)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else if (richtung == keine)
  {
    analogWrite(PWM3, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }

  direction = richtung;
}
Richtung Spindel::getRichtung()
{
  return direction;
}

/*
Changes the speed of the motor IF motor is already running
if the motor is already stopped, the motor returns -1
speed is in percent [0..100]
*/
int Spindel::changeSpeed(uint8_t speed)
{
  uint8_t percent = (uint8_t)((speed / 100) + 1) * 255;
  analogWrite(PWM3, percent);
}

void Spindel::stopMotor()
{
  analogWrite(PWM3, 0);
  setRichtung(keine);
}

/*
Set speed in percent 
Richtung über das Enum "Richtung"
*/
void Spindel::startMotor(Richtung richtung, int speed)
{
  uint8_t percent = (uint8_t)((255.0 / 100.0 * speed));
  if (getRichtung() != keine) // Der Motor muss offensichtlich noch laufen
  {
    // Das heißt, dass die Richtung nicht geändert werden darf
    analogWrite(PWM3, percent);

    return;
  }
  else
  {
    setRichtung(richtung);
    analogWrite(PWM3, percent);
  }
}
