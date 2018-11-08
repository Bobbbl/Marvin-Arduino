#include <Arduino.h>
#include <Stepper.h>

#define PWM1 5
#define PWM2 6
#define DIR1 13
#define DIR2 12

#define NUMBER_OF_STEPS_X 200
#define NUMBER_OF_STEPS_Y 200

unsigned long last_step_time_x, last_step_time_y;
unsigned long step_delay_x, step_delay_y;
int direction_x, direction_y;
int step_number_x, step_number_y;


void stepXY(int steps_to_move_x, int steps_to_move_y);

void setup() {
  // Init Serial Port: Fast Baud Rate is adviced
  Serial.begin(115200);
  // No Fast and precice PWM Frequency is necessary, so no PWM
  // Timer is adviced
  pinMode(PWM1, OUTPUT),
  pinMode(PWM2, OUTPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);


  

}

void loop() {
  // put your main code here, to run repeatedly:

}

void stepXY(int steps_to_move_x, int steps_to_move_y){
  int steps_left_x = abs(steps_to_move_x);
  int steps_left_y = abs(steps_to_move_y);

  if(steps_to_move_x > 0) {
    direction_x = 1;
  }
  else if(steps_to_move_x < 0){
    direction_x = 0;
  }

  
  if(steps_to_move_y > 0) {
    direction_y = 1;
  }
  else if(steps_to_move_y < 0){
    direction_y = 0;
  }

  while(steps_left_x > 0 && steps_left_y > 0){
    unsigned long now = micros();
    if(now - last_step_time_x >= step_delay_x){
      last_step_time_x = now;
      
      if(direction_x == 1){
        step_number_x++;
        if(step_number_x == NUMBER_OF_STEPS_X){
          step_number_x = 0;
        }
      }
    }
  }

}