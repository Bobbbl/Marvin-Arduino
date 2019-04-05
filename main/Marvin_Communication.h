#ifndef MARVIN_COMMUNICATION
#define MARVIN_COMMUNICATION

#include "Arduino.h"
#include <string.h>
#include "Pin_Defines.h"
#include "Marvin_Motor.h"
#include "Druckpumpe.h"

#define dict_length 20
#define MESSAGE_TIMEOUT (unsigned long)5000  // Timeout in ms


using namespace std;

// Capsulate up to 10 Strings in this Struct
struct StringArray {
	int count = 0;
	char str_array[10][20];
};

enum cE
{
	XYF = 0x00,
	S = 0x01,
	CS = 0x02,
	P = 0x03,
	Wait = 0x04,
	Z = 0x05,
	STOP = 0x06,
	NO_VALID_MESSAGE = 0x07,
	SPM = 0x08,
	KPKDKI = 0x09
};
typedef enum cE commEnum;

typedef struct
{
	commEnum Command;
	double T1;
	double T2;
	double T3;
}Commando;

typedef enum {
	Start_Session = 0,
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
void sendACK(commEnum c, Commando com);
Commando createCommand(commEnum c, StringArray xm);
uint8_t runCommand(Commando com, PressurePump *pump, Spindel *spindel, Marvin_Steppers *stepper_motors);
void setAnteil(double kp, double kd, double ki);


struct StringArray getValueInArray(String data, char separator);
commEnum GetCommunicationEnum(String str);


#endif