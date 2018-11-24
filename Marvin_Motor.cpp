#include "Marvin_Motor.h"

volatile uint16_t steps_x = 0, steps_y = 0;

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

void Marvin_Steppers::stepMotorX(Richtung r){
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




void Marvin_Steppers::stepMotorY(Richtung r){
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


void Marvin_Steppers::setSpeed_x(long whatSpeed){
    this->step_delay_x = 60L * 1000L * 1000L / this->number_of_steps_x / whatSpeed;
}


void Marvin_Steppers::setSpeed_y(long whatSpeed){
    this->step_delay_y = 60L * 1000L * 1000L / this->number_of_steps_y / whatSpeed;
}

void Marvin_Steppers::stepPWM(Strecke_Steps_RPM s){
  // Set speed X
  // Formel: MAXCOUNT - Compare Match Time*CPUFRQ/Prescaler
  float spm = 65536 - s.rpm_x * this->number_of_steps_x;
  float ips = 2 * spm;
  float cmt = 1 / ips;
  float count = 65536 - cmt * 16000000L / this->prescaler;
  uint16_t round_count = (uint16_t)round(count);
  OCR3A = round_count;
  // Set speed Y
  spm =  65536 - s.rpm_y * this->number_of_steps_y;
  ips = 2 * spm;
  cmt = 1 / ips;
  count = 65536 - cmt * 16000000L / this->prescaler;
  OCR2A = round_count;
  // Set Steps
  if(s.steps_x < 0)
  {
    this->setDirectionMotorX("rechts");
    steps_x = -s.steps_x;
  }else{
    this->setDirectionMotorX("links");
    steps_x = s.steps_y;
  }

  if(s.steps_y < 0)
  {
    this->setDirectionMotorY("rechts");
    steps_y = -s.steps_y;
  }else{
    this->setDirectionMotorY("links");
    steps_y = s.steps_y;
  }

  this->startTimer3();
  this->startTimer4();
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

void Marvin_Steppers::stopTimer3(){
  // Stop Timer
  //TCCR3B &=~ (1 << CS32) | (1 << CS31) | (1 << CS30);
  TCCR3B = 0x00;
}


void Marvin_Steppers::startTimer3(){
  // Start Timer 
    TCCR3B |= (1 << CS31); // Prescaler 8
}

void Marvin_Steppers::stopTimer4(){
  // Stop Timer
  //TCCR4B &=~ (1 << CS42) | (1 << CS41) | (1 << CS40);
  TCCR4B = 0x00;
}


void Marvin_Steppers::startTimer4(){
  // Start Timer 
    TCCR4B |= (1 << CS41); // Prescaler 8
}

void Marvin_Steppers::setDirectionMotorX(char* str)
{
  if (strcmp("rechts", str) == 0)
  {
    digitalWrite(DIR1, HIGH);
  }else if(strcmp("links", str) == 0){
    digitalWrite(DIR1, LOW);
  }
}


void Marvin_Steppers::setDirectionMotorY(char* str)
{
  if (strcmp("rechts", str) == 0)
  {
    digitalWrite(DIR2, HIGH);
  }else if(strcmp("links", str) == 0){
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

  return ssp;
}