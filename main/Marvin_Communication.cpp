#include "Marvin_Communication.h"
#include "Arduino.h"


#define DEBUG

extern volatile long steps_per_millimeter;
extern double Setpoint;
extern bool running_flag;

String Sanchezcomm_dict[COMM_LENGTH] = {
	"__Start_Session__",
	"__End_Session__",
	"__Start_Homing__",
	"__Send_Toolpath__",
	"__Empfang_Besteatigt__",
	"__Receive_Error__",
	"__Receive_Successfull__",
	"__Point_Reached__" };

uint8_t checkConnection()
{
	return Serial.available();
}

communication_alphabet checkForValidMessage()
{
	// Wait for Message available in Buffer
	// Wenn keine Message gelesen werden kann, dann
	// dann wird einfach ein "No Message" zurückgegeben
	for (int j = 0; j <= 2; j++)
	{
		if (Serial.available() != 0)
		{
			break;
		}

		delay(100);

		if (Serial.available() != 0)
		{
			break;
		}
	}
	// I use the "readString" - Function only because the
	// the String Sequences will be short and Memory Frakmentation
	// will be no Problem
	String m = Serial.readString();
	// Check if Message is Part of registered Messages
	for (int i = 0; i <= COMM_LENGTH - 1; i++)
	{
		if (m == Sanchezcomm_dict[i])
		{
			return (communication_alphabet)i;
		}
	}

	return No_Message;
}

communication_alphabet waitForSession()
{
	communication_alphabet message = No_Message;
	do
	{
		message = checkForValidMessage();
		// Send NACK
		if (message != No_Message)
		{
			sendNack();
		}
	} while (message == No_Message);
}

communication_alphabet waitForKonsekutiveMessage()
{
	communication_alphabet message = No_Message;
	unsigned long first = millis(); // Saves Time of First Entering Function for Timeout Function
	unsigned long now = millis();   // Variable for This Loop Time
	// Enter Waiting Loop
	while (message == No_Message)
	{
		now = millis();
		message = checkForValidMessage();
		// Check For Timeout
		if ((now - first) >= 1000)
		{
			return No_Message;
		}
	}

	// Send NACK
	sendNack();
	return message;
}

void sendNack()
{
	Serial.println(Sanchezcomm_dict[Empfang_Bestaetigt]);
}

void sendEndSession()
{
	Serial.println(Sanchezcomm_dict[End_Session]);
}

String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = { 0, -1 };
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (data.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

commEnum GetCommunicationEnum(String str)
{
	commEnum c;

	if (str.indexOf("XYF") >= 0)
	{
		return XYF;
	}
	else if (str.indexOf("PID") >= 0)
	{
		return KPKDKI;
	}
	else if (str.indexOf("SPM") >= 0)
	{
		return SPM;
	}
	else if (str.indexOf("STOP") >= 0)
	{
		return STOP;
	}
	else if (str.indexOf("CS") >= 0)
	{
		return CS;
	}
	else if (str.indexOf("S") >= 0)
	{
		return S;
	}
	else if (str.indexOf("P") >= 0)
	{
		return P;
	}
	else if (str.indexOf("Z") >= 0)
	{
		return Z;
	}
	else if (str.indexOf("Wait") >= 0)
	{
		return Wait;
	}
	else
	{
		return NO_VALID_MESSAGE;
	}
}

// struct StringArray getValueInArray(String data, char separator)
// {
//     int found = 0;
//     int strIndex[] = {0, -1};
//     int maxIndex = data.length() - 1;
//     struct StringArray strarray;

//     for (int i = 0; i <= maxIndex; i++)
//     {
//         if (data.charAt(i) == separator || i == maxIndex)
//         {
//             found++;
//         }
//     }
//     strarray.count = found;
//     found = 0;

//     for (int i = 0; i <= maxIndex; i++)
//     {
//         if (data.charAt(i) == separator || i == maxIndex)
//         {
//             found++;
//             strIndex[0] = strIndex[1] + 1;
//             strIndex[1] = (i == maxIndex) ? i + 1 : i;
//             strarray.str_array[i] = data.substring(strIndex[0], strIndex[1]);
//         }
//     }

//     return strarray;
// }

Strecke readToolPathLine()
{
	String line;
	String xstr, ystr, fstr;
	float x, y, f;
	Strecke r = { .x = 0,.y = 0,.f = 0,.error = 0,.end_session = 0 };
	// Read New Line
	line = Serial.readString();
	communication_alphabet message = No_Message;
	// Check if Line is Communication Alphabet Message (like End_Session)
	// which would mean, that transmission would be over
	for (int i = 0; i <= COMM_LENGTH - 1; i++)
	{
		if (line == Sanchezcomm_dict[i])
		{
			message = (communication_alphabet)i;
		}
	}
	if (message != No_Message)
	{
		if (message == End_Session)
		{
			// Session was ended by protokoll
			// No error
			r.end_session = 1;
		}
	}

	// Search for the indices of X, Y and F
	// if one of them is missing, there must be
	// an error - so, cancel session and reset
	// communication
	uint8_t pos_X, pos_Y, pos_F, pos_S, pos_RS, pos_P;
	pos_X = line.indexOf('X');
	pos_Y = line.indexOf('Y');
	pos_F = line.indexOf('F');

	if (pos_Y == 0 || pos_F == 0)
	{
		// There must be an error
		r.error = 1;
		return r;
	}
	// All went as expected - so break up the line in substrings

	xstr = line.substring(pos_X, pos_Y);
	ystr = line.substring(pos_Y, pos_F);
	fstr = line.substring(pos_F);

	// Now remove all leadig and trailing whitespaces
	xstr.trim();
	ystr.trim();
	fstr.trim();
	// Now remove the first character (the X,Y,F) from each string
	xstr.remove(0, 1);
	ystr.remove(0, 1);
	fstr.remove(0, 1);
	// Convert the strings to floats
	x = xstr.toFloat();
	y = ystr.toFloat();
	f = fstr.toFloat();
	r.x = x;
	r.y = y;
	r.f = f;
	return r;
}

// Returns -1 for line error
//          0 for no error and no end_session
//          1 for end message
Strecke receiveKoordinate()
{
	Strecke s = { .x = 0,.y = 0,.f = 0,.error = 1,.end_session = 0 };
	unsigned long now = millis();
	do
	{
		if (Serial.available() != 0)
		{
			s = readToolPathLine();
			if (s.end_session == 0 && s.error == 0)
			{
				return s;
			}
			else if (s.end_session == 0)
			{
				return s;
			}
		}
	} while (millis() - now < MESSAGE_TIMEOUT);
	return s;
}

Strecke receivePoint()
{
	Strecke s = receiveKoordinate();
	// If Error occured, send Error and send End session
	if (s.error == 1)
	{
		SanchezsendError();
		sendEndSession();
		return s;
	}
	else if (s.end_session == 1)
	{
		sendEndSession();
		return s;
	}
	else if (s.end_session == 0 && s.error == 0)
	{
		// Inform Client that everything went as expected
		sendReceiveSuccessfull();
		return s;
	}
}

void SanchezsendError()
{
	Serial.println(Sanchezcomm_dict[Receive_Error]);
}

void sendReceiveSuccessfull()
{
	Serial.println(Sanchezcomm_dict[Receive_Successfull]);
}

void sendPointReached()
{
	Serial.println(Sanchezcomm_dict[Point_Reached]);
}


Commando createCommand(commEnum c, StringArray xm)
{
	Commando com;
	com.Command = c;
	com.T1 = -1;
	com.T2 = -1;
	com.T3 = -1;

	switch (c)
	{
	case XYF:
		com.T1 = (float)atof(xm.str_array[1]);
		com.T2 = (float)atof(xm.str_array[2]);
		com.T3 = (float)atof(xm.str_array[3]);
		break;
	case KPKDKI:
		com.T1 = (float)atof(xm.str_array[1]);
		com.T2 = (float)atof(xm.str_array[2]);
		com.T3 = (float)atof(xm.str_array[3]);
		break;
	default:
		com.T1 = (float)atof(xm.str_array[1]);
		break;
	}

	return com;
}

void sendACK(commEnum c, Commando com)
{
	switch (c)
	{
	case XYF:
		Serial.print("ACK");
		Serial.print(" XYF");

		Serial.print(" ");
		Serial.print(com.T1);
		Serial.print(" ");
		Serial.print(com.T2);
		Serial.print(" ");
		Serial.println(com.T3);
		break;
	case KPKDKI:
		Serial.print("ACK");
		Serial.print(" PID");

		Serial.print(" ");
		Serial.print(com.T1);
		Serial.print(" ");
		Serial.print(com.T2);
		Serial.print(" ");
		Serial.println(com.T3);
		break;
	case NO_VALID_MESSAGE:
		Serial.println("NACK");
	default:
		Serial.print("ACK");

		Serial.print(" ");
		Serial.println(com.T1);
		break;
	}
	Serial.println();
}

void setAnteil(double Kp, double Kd, double Ki)
{
	/*kp = Kp;
	kd = Kd;
	ki = Ki;*/
}

// Returns  1 for successfull command
//			0 for waiting for completion
//			-1 for not successfull completion
uint8_t runCommand(Commando com, PressurePump *pump, Spindel *spindel, Marvin_Steppers *stepper_motors)
{
	static long timeold = millis();
	static bool running = false;


	switch (com.Command)
	{
	case Wait:

		if (running)
		{
			if (millis() - timeold >= com.T1)
			{
				String answer = String("Delay Ended");
				Serial.println(answer);
				running = false;
				return 1;
			}
			return 0;
		}
		else
		{
			running = true;
			timeold = millis();
			return 0;
		}
	case CS:
		Setpoint = com.T1;
		return 1;
	case P:
		/*Start Motor mit */
		pump->startMotor(com.T1);
		return 1;
	case Z:
		if (com.T1 == 0)
		{
			digitalWrite(RELAY_IN4, HIGH);
		}
		else
		{
			digitalWrite(RELAY_IN4, LOW);
		}
		return 1;
		break;
	case STOP:
		/*Stop Spindel*/
		spindel->stopMotor();
		/*Stop Pump*/
		pump->stopMotor();
		/*Stop Stepper Motors*/
		stepper_motors->stopMotors();
		return 1;
	case SPM:
		/*Set Steps per millimeter (extern variable)*/
		steps_per_millimeter = com.T1;
		return 1;
	case XYF:
		/*Wenn Toolpath noch nicht erreicht wurde, dann
		dann gib eine 0 zurück und lass den Pointer nicht
		weiter wandern. Damit wird immer wieder der geliche
		Befehl ausgeführt, bis running_flag == true*/
		if (running == true)
		{
			if (running_flag == false)
			{
				/*Der Toolpath wurde noch nicht erreicht*/
				running = false;
				Serial.println("Reached");
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			/*Start Breshenham (Timer 3)*/
			Strecke s;
			s.x = com.T1;
			s.y = com.T2;
			s.f = com.T3;
			stepper_motors->bresenham(s, steps_per_millimeter);
			running_flag = true;
			running = true;
			return 0;

		}
	case KPKDKI:
		/*
		Setze Anteile für PID Regler in Reihenfolge
		Proportionalanteil
		Differentialanteil
		Integralanteil
		*/
		Serial.println("KPKDKI");
		return 1;
	case NO_VALID_MESSAGE:
		/*Keine bekannte Message wurde gesendet - Gib -1 zurück*/
		Serial.println("No Valid Message");
		return -1;
	default:
		/*Zur Sicherheit, falls doch einmal etwas schief gehen sollte*/
		Serial.println("Default");
		return -1;
	}

}