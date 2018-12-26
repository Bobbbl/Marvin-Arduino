#include "Marvin_Communication.h"

#define DEBUG

uint8_t checkConnection(){
    return Serial.available();
}



 communication_alphabet checkForValidMessage(){
    // Wait for Message available in Buffer 
    // Wenn keine Message gelesen werden kann, dann
    // dann wird einfach ein "No Message" zurückgegeben
    for(int j = 0; j<=2;j++)
    {
        if(Serial.available() != 0)
        {
            break;
        }

        delay(100);

        if(Serial.available() != 0)
        {
            break;
        }
    }
    // I use the "readString" - Function only because the
    // the String Sequences will be short and Memory Frakmentation
    // will be no Problem
    String m = Serial.readString();
    // Check if Message is Part of registered Messages
    for(int i = 0; i<=COMM_LENGTH-1; i++){
        if(m == comm_dict[i])
        {
            return (communication_alphabet)i;
        }
    }
    
    return No_Message;
}



communication_alphabet waitForSession(){
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


communication_alphabet waitForKonsekutiveMessage(){
    communication_alphabet message = No_Message;
    unsigned long first = millis(); // Saves Time of First Entering Function for Timeout Function
    unsigned long now = millis();   // Variable for This Loop Time
    // Enter Waiting Loop
    while (message == No_Message){
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


inline void sendNack(){
    Serial.println(comm_dict[Empfang_Bestaetigt]);
}


inline void sendEndSession()
{
    Serial.println(comm_dict[End_Session]);
}


Strecke readToolPathLine()
{
    String line;
    String xstr, ystr, fstr;
    float x, y, f;
    Strecke r = {.x = 0, .y = 0, .f = 0, .error = 0, .end_session = 0};
    // Read New Line
    line = Serial.readString();
    communication_alphabet message = No_Message;
    // Check if Line is Communication Alphabet Message (like End_Session)
    // which would mean, that transmission would be over
    for (int i = 0; i <= COMM_LENGTH-1; i++)
    {
        if (line == comm_dict[i])
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
    uint8_t pos_X, pos_Y, pos_F;
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
Strecke receiveKoordinate(){
    Strecke s = {.x = 0, .y = 0, .f = 0, .error = 1, .end_session = 0};
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
            else if(s.end_session == 0)
            {
               return s; 
            }
        }
    } while (millis() - now < MESSAGE_TIMEOUT);
    return s;
}

Strecke receivePoint(){
    Strecke s = receiveKoordinate();
    // If Error occured, send Error and send End session
    if (s.error == 1)
    {
        sendError();
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


inline void sendError(){
    Serial.println(comm_dict[Receive_Error]);
}

inline void sendReceiveSuccessfull(){
    Serial.println(comm_dict[Receive_Successfull]);
}

inline void sendPointReached(){
    Serial.println(comm_dict[Point_Reached]);
}