#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include <Arduino.h>
#include <string.h>

#define dict_length 20

using namespace std;

enum {Start_Session, 
    End_Session,
    Start_Homing,
    Send_Toolpath
    };

String comm_dict[20]= {
    "__Start_Session__",
    "__End_Session__",
    "__Start_Homing__",
    "__Send_Toolpath__"
};

uint8_t checkConnection();
uint8_t waitForValidMessage(char* sentMessage);

#endif