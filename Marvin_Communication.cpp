#include "Marvin_Communication.h"



uint8_t checkConnection(){
    return Serial.available();
}



 communication_alphabet checkForValidMessage(){
    // Wait for Message available in Buffer 
    while(Serial.available() == 0);
    String m = Serial.readString();
    // Check if Message is Part of registered Messages
    for(int i = 0; i<=sizeof(comm_dict)/sizeof(comm_dict[0]); i++){
        if(m.compareTo(comm_dict[i])){
            return (communication_alphabet)i;
        }
    }
    
    return No_Message;
}



communication_alphabet waitForSession(){
    communication_alphabet message = No_Message;
    while(message == -1){
        communication_alphabet message;
        message = checkForValidMessage();
    }
}


communication_alphabet waitForKonsekutiveMessage(){
    communication_alphabet message = No_Message;
    unsigned long first = millis(); // Saves Time of First Entering Function for Timeout Function
    unsigned long now = millis();   // Variable for This Loop Time
    // Enter Waiting Loop
    while(message == -1){
        now = millis();
        communication_alphabet message;
        message = checkForValidMessage();
        // Check For Timeout
        if((now - first) >= 1000){
            break;
        }
    }
}