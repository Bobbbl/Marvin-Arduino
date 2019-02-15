#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include "Arduino.h"
#include <string.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"

#define dict_length 20
#define MESSAGE_TIMEOUT (unsigned long)5000  // Timeout in ms

using namespace std;

enum cE {XYF, S, P, Wait, Z, NO_VALID_MESSAGE};
typedef enum cE commEnum;

typedef enum {Start_Session = 0, 
End_Session = 1,
Start_Homing = 2,
Send_Toolpath = 3,
Empfang_Bestaetigt = 4,
Receive_Error = 5,
Receive_Successfull = 6,
Point_Reached = 7,
No_Message = -1
}communication_alphabet;

#define COMM_LENGTH 8

extern String Sanchezcomm_dict[COMM_LENGTH];



uint8_t checkConnection();
communication_alphabet checkForValidMessage();
communication_alphabet waitForSession();
communication_alphabet waitForKonsekutiveMessage();
Strecke readToolPathLine();
Strecke receiveKoordinate();
Strecke receivePoint();
String getValue(String data, char separator, int index);

// Communication shortcuts
void sendNack();
void sendEndSession();
void SanchezsendError();
void sendReceiveSuccessfull();
void sendPointReached();

// Capsulate up to 10 Strings in this Struct
struct StringArray{
    int count = 0;
    char str_array[10][20];
};
struct StringArray getValueInArray(String data, char separator);
commEnum GetCommunicationEnum(String str);


#endif