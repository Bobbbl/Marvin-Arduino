#ifndef MARVIN_MOTOR
#define MARVIN_MOTOR

#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "Pin_Defines.h"

extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;

#define STEPS_PER_REVOLUTION_X 25600
#define STEPS_PER_REVOLUTION_Y 25600
#define DISTANCE_PER_REVOLUTION_X 0.5 // Strecke pro Umdrehung in mm
#define DISTANCE_PER_REVOLUTION_Y 0.5 // Strecke pro Umdrehung in mm

#define STEPS_PER_MILLIMETER_X (float)400.00 // Wieviele Schritte ergeben einen Millimeter X
#define STEPS_PER_MILLIMETER_Y (float)400.00 // Vieviele Schritte ergeben einen Millimeter Y

#define CPU_FREQ (unsigned long)16000000

typedef enum
{
  links = 0,
  rechts = 1,
  keine = 2
} Richtung;

typedef struct
{
  float x;
  float y;
  float f;

  uint8_t error;
  uint8_t end_session;
} Strecke;

typedef struct
{
  int steps_x;
  int steps_y;
  float rpm_x;
  float rpm_y;

  uint8_t error;
  uint8_t end_session;
} Strecke_Steps_RPM;

typedef struct
{
  float x;
  float y;
} Point;

typedef struct
{
  float x;
  float y;
} Vector;

Strecke_Steps_RPM convertToStepsAndRPM(Strecke s);

class Spindel
{
public:
  Spindel()
  {
    // Pin Output Modes 
    pinMode(PWM3, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // All Pins to Low
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(PWM3, 0);
  }

  void setRichtung(Richtung);
  Richtung getRichtung();
  void startMotor(Richtung richtung, int speed);
  void stopMotor();
  int changeSpeed(uint8_t speed);

private:
  Richtung direction = keine;


};

class Marvin_Steppers
{

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
    // noInterrupts();
    // TCCR3A = 0;
    // TCCR3B = 0;
    // TCNT3 = 0;
    // TCCR3A = 0;
    // TCCR3B = 0;
    // TCCR4A = 0;
    // TCCR4B = 0;
    // // Waveform Generation Mode
    // TCCR3A |= (1 << WGM32); // CTC Mode
    // // Compare Output Mode
    // // TCCR3A |= (1 << COM3A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // // TCCR3A &= ~(1 << COM3A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // // Prescaler
    // this->prescaler = 64;
    // // TCCR3B |= (1 << CS31); // Prescaler 8
    // // Interrupts
    // TIMSK3 |= (1 << OCIE3A); // Output Compare Interrupt Enabled
    // OCR3A = 0;

    // TCCR4A = 0;
    // TCCR4B = 0;
    // TCNT4 = 0;
    // // Waveform Generation Mode
    // TCCR4A |= (1 << WGM42); // CTC Mode
    // // Compare Output Mode
    // // TCCR4A |= (1 << COM4A0);  // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // // TCCR4A &= ~(1 << COM4A1); // Toggle OC3A (PE3) bzw. PWM 6 on Compare Match
    // // Prescaler
    // this->prescaler = 64;
    // //TCCR4B |= (1 << CS41); // Prescaler 8
    // // Interrupts
    // TIMSK4 |= (1 << OCIE4A); // Output Compare Interrupt Enabled
    // OCR4A = 0;
    // interrupts();
  }

  void stepPWM(Strecke_Steps_RPM s);
  void easyStep(Strecke s);
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
  void startTimer3(unsigned long prescaler);
  void stopTimer3();
  void startTimer4(unsigned long prescaler);
  void stopTimer4();
  void setDirectionMotorX(char *str);
  void setDirectionMotorY(char *str);
  void bresenham(Strecke s);
  void tt(Strecke s);
  int version();

private:
  void stopTimer();
  int direction_x, direction_y;
  unsigned long step_delay_x, step_delay_y; // delay between steps, in ms
  int step_number_x, step_number_y;
  unsigned long last_step_time_x, last_step_time_y;
  int pwm_x, pwm_y;
  int dir_x, dir_y;
};

#endif