#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include <Arduino.h>
#include <string.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"

#define dict_length 20
#define MESSAGE_TIMEOUT (unsigned long)500  // Timeout in ms

using namespace std;

typedef enum {Start_Session = 0, 
End_Session = 1,
Start_Homing = 2,
Send_Toolpath = 3,
Empfang_Bestaetigt = 4,
Receive_Error = 5,
Receive_Successfull = 6,
No_Message = -1
}communication_alphabet;

String comm_dict[20]= {
    "__Start_Session__",
    "__End_Session__",
    "__Start_Homing__",
    "__Send_Toolpath__",
    "__Empfang_Besteatigt__",
    "__Receive_Error__",
    "__Receive_Successfull__"
};



uint8_t checkConnection();
communication_alphabet checkForValidMessage();
communication_alphabet waitForSession();
communication_alphabet waitForKonsekutiveMessage();
Strecke readToolPathLine();
Strecke receiveKoordinate();
Strecke receivePoint();

// Communication shortcuts
inline void sendNACK();
inline void sendEndSession();
inline void sendError();
inline void sendReceiveSuccessfull();
#endif