#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include <Arduino.h>

enum {Start_Session, 
    End_Session,
    Start_Homing,
    Send_Toolpath
    };

uint8_t comm_dict[] = {
    "__Start_Session__",
    "__End_Session__",
    "__Start_Homing__",
    "__Send_Toolpath__"
};

#endif