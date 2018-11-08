#include "Marvin_Motor.h"



void Marvin_Steppers::stepMotorX(){
  digitalWrite(this->pwm_x, !digitalRead(this->pwm_x));
}
void Marvin_Steppers::stepMotorY(){
  digitalWrite(this->pwm_y, !digitalRead(this->pwm_y));
}


void Marvin_Steppers::setSpeed_x(long whatSpeed){
    this->step_delay_x = 60L * 1000L * 1000L / this->number_of_steps_x / whatSpeed;
}


void Marvin_Steppers::setSpeed_y(long whatSpeed){
    this->step_delay_y = 60L * 1000L * 1000L / this->number_of_steps_y / whatSpeed;
}


void Marvin_Steppers::step(int steps_to_move_x, int steps_to_move_y)
{
  int steps_left_x = abs(steps_to_move_x);  // how many steps to take
  
  int steps_left_y = abs(steps_to_move_y);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move_x > 0) { this->direction_x = 1; }
  if (steps_to_move_x < 0) { this->direction_x = 0; }
  

  if (steps_to_move_y > 0) { this->direction_y = 1; }
  if (steps_to_move_y < 0) { this->direction_y = 0; }


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
        if (this->step_number_x == this->number_of_steps_x) {
          this->step_number_x = 0;
        }
      }
      else
      {
        if (this->step_number_x == 0) {
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
        if (this->step_number_y == this->number_of_steps_y) {
          this->step_number_y = 0;
        }
      }
      else
      {
        if (this->step_number_y == 0) {
          this->step_number_y = this->number_of_steps_y;
        }
        this->step_number_y--;
      }
      // decrement the steps left:
      steps_left_y--;
    }
  }

}