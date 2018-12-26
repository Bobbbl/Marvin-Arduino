#include "Marvin_Motor.h"

volatile int longpin, shortpin;
volatile float bcount = 0;
volatile uint16_t steps_x = 0, steps_y = 0;
volatile unsigned long pulses_x = 0, pulses_y = 0;

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
    this->setDirectionMotorX("rechts");
    steps_x = -s.steps_x;
  }
  else
  {
    this->setDirectionMotorX("links");
    steps_x = s.steps_y;
  }

  if (s.steps_y < 0)
  {
    this->setDirectionMotorY("rechts");
    steps_y = -s.steps_y;
  }
  else
  {
    this->setDirectionMotorY("links");
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
    this->setDirectionMotorX("rechts");
    pulses_x = pulsesx;
  }
  else
  {
    this->setDirectionMotorX("links");
    pulses_x = pulsesx;
  }

  if (v.y < 0)
  {
    this->setDirectionMotorY("rechts");
    pulses_y = pulsesy;
  }
  else
  {
    this->setDirectionMotorY("links");
    pulses_y = pulsesy;
  }

  lastpoint.x = thispoint.x;
  lastpoint.y = thispoint.y;

  this->startTimer3(prescaler);
  this->startTimer4(prescaler);
}

void Marvin_Steppers::bresenham(Strecke s)
{
  this->prescaler = 256;

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

  bcount = round(longline / shortline);

  // Länge Vektor
  float l = sqrt(v.x * v.x + v.y * v.y);

  // Zeit in Sekunden
  float t = l / s.f * 60.0;

  // Zeit pro Schritt
  float st = t / longline;

  // Compare Match Count
  float count = st * 16000000.00 * this->prescaler;
  // Count auf Ganzzahl runden
  unsigned long rcount = round(count);

  // Den Compare - Wert setzen
  OCR4A = rcount;
  Serial.println(rcount);

  pulses_x = longline;
  pulses_y = shortline;


  // Timer starten
  this->startTimer3(this->prescaler);
}

void Marvin_Steppers::tt(Strecke s){
  
  this->prescaler = 256;

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
  float hzx = 1/stx;
  float hzy = 1/sty; 
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
  cli();
  // Stop Timer
  TCCR3B &= ~((1 << CS32) | (1 << CS31) | (1 << CS30));
  // TCCR3B = 0x00;
  sei();
}

void Marvin_Steppers::startTimer3(unsigned long prescaler)
{
  // Start Timer
    TCCR3A |= (1 << WGM32); // CTC Mode
    DDRE |= (1<<3);
    // Compare Output Mode
    TCCR3A |= (1 << COM3A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    TCCR3A &= ~(1 << COM3A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // Prescaler
    this->prescaler = prescaler;
    // TCCR3B |= (1 << CS31); // Prescaler 8
    // Interrupts
    TIMSK3 |= (1 << OCIE3A); // Output Compare Interrupt Enabled
  // TCCR3B |= (1 << CS32); // Prescaler 256

  switch (prescaler)
  {
  case 1:
    TCCR3B |= (1 << CS30); // Prescaler 1
    this->prescaler = 1;
    break;

  case 8:
    TCCR3B |= (1 << CS31); // Prescaler 8
    this->prescaler = 8;
    break;

  case 64:
    TCCR3B |= ((1 << CS30) | (1 << CS31)); // Prescaler 64
    this->prescaler = 64;
    break;

  case 256:
    TCCR3B |= (1 << CS32); // Prescaler 256
    this->prescaler = 256;
    break;

  case 1024:
    TCCR3B |= ((1 << CS30) | (1 << CS32)); // Prescaler 1024
    this->prescaler = 1024;
    break;

  default:
    break;
  }

}

void Marvin_Steppers::stopTimer4()
{
  // Stop Timer
  cli();
  TCCR4B &= ~((1 << CS42) | (1 << CS41) | (1 << CS40));
  sei();
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