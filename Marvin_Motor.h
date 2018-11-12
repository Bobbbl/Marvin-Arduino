#ifndef MARVIN_MOTOR
#define MARVIN_MOTOR

#include <Arduino.h>
#include "Pin_Defines.h"

extern uint8_t endschalter_flag_x;
extern uint8_t endschalter_flag_y;

typedef enum {
    links = 0,
    rechts = 1
} Richtung;


class Marvin_Steppers{

  public:
    Marvin_Steppers(unsigned int PWM_x, unsigned int PWM_y, unsigned int DIR_x, unsigned int DIR_y) : pwm_x(PWM_x),
                                                                                                      pwm_y(PWM_y),
                                                                                                      dir_x(DIR_x),
                                                                                                      dir_y(DIR_y) {}

    void setSpeed_x(long whatSpeed);
    void setSpeed_y(long whatSpeed);
    void step(int steps_to_move_x, int steps_to_move_y);
    void stepMotorX(Richtung r);
    void stepMotorY(Richtung r);
    void checkLimitSwitches();
    void doHoming();
    uint8_t x_flag;
    uint8_t y_flag;

    int version();

  private:
    int direction_x, direction_y;
    unsigned long step_delay_x, step_delay_y; // delay between steps, in ms
    int number_of_steps_x, number_of_steps_y; // total number of steps per revolution
    int step_number_x, step_number_y;
    unsigned long last_step_time_x, last_step_time_y;
    int pwm_x, pwm_y;
    int dir_x, dir_y;
};


#endif