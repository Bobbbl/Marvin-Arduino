#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include <Arduino.h>
#include <string.h>

#define dict_length 20

using namespace std;

typedef enum {Start_Session = 0, 
End_Session = 1,
Start_Homing = 2,
Send_Toolpath = 3,
Empfang_Bestaetigt = 4,
No_Message = -1
}communication_alphabet;

String comm_dict[20]= {
    "__Start_Session__",
    "__End_Session__",
    "__Start_Homing__",
    "__Send_Toolpath__",
    "__Empfang_Besteatigt__"
};

uint8_t checkConnection();
communication_alphabet checkForValidMessage();
communication_alphabet waitForSession();
communication_alphabet waitForKonsekutiveMessage();
#endif