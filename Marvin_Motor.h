#ifndef MARVIN_MOTOR
#define MARVIN_MOTOR

#include <Arduino.h>
#include "Pin_Defines.h"
#include <math.h>


extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;
volatile steps_x = 0, steps_y = 0;

typedef enum {
    links = 0,
    rechts = 1
} Richtung;


class Marvin_Steppers{

public:
  Marvin_Steppers(unsigned int PWM_x, unsigned int PWM_y, unsigned int DIR_x, unsigned int DIR_y) : pwm_x(PWM_x),
                                                                                                    pwm_y(PWM_y),
                                                                                                    dir_x(DIR_x),
                                                                                                    dir_y(DIR_y)
  {
    this->number_of_steps_x = 200;
    this->number_of_steps_y = 200;
    // Timer is Timer 3 - OC3A is Pin 6 PWM
    // Disable all Interrupts
    noInterrupts();
    TCCR3A = 0;
    TCCR3B = 0;
    TCNT3 = 0;
    // Waveform Generation Mode
    TCCR3A |= (1 << WGM32); // CTC Mode
    // Compare Output Mode
    TCCR3A |= (1 << COM3A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    TCCR3A &= ~(1 << COM3A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // Prescaler
    this->prescaler = 8;
    TCCR3B |= (1 << CS31); // Prescaler 8
    // Interrupts
    TIMSK3 |= (1 << OCIE3A); // Output Compare Interrupt Enabled
    OCR3A = 0;

    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0;
    // Waveform Generation Mode
    TCCR4A |= (1 << WGM42); // CTC Mode
    // Compare Output Mode
    TCCR4A |= (1 << COM4A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    TCCR4A &= ~(1 << COM4A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // Prescaler
    this->prescaler = 8;
    TCCR4B |= (1 << CS41); // Prescaler 8
    // Interrupts
    TIMSK4 |= (1 << OCIE4A); // Output Compare Interrupt Enabled
    OCR4A = 0;
    interrupts();
  }

  void stepPWM(int steps_to_move_x, int steps_to_move_y, float speed_x, float speed_y);
  void setSpeed_x(long whatSpeed);
  void setSpeed_y(long whatSpeed);
  void step(int steps_to_move_x, int steps_to_move_y);
  void stepMotorX(Richtung r);
  void stepMotorY(Richtung r);
  void checkLimitSwitches();
  void doHoming();
  uint8_t x_flag;
  uint8_t y_flag;
  uint16_t prescaler;
  int number_of_steps_x, number_of_steps_y; // total number of steps per revolution

  int version();

private:
  int direction_x, direction_y;
  unsigned long step_delay_x, step_delay_y; // delay between steps, in ms
  int step_number_x, step_number_y;
  unsigned long last_step_time_x, last_step_time_y;
  int pwm_x, pwm_y;
  int dir_x, dir_y;
};


#endif