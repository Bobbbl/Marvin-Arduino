#include "Marvin_Communication.h"



uint8_t checkConnection(){
    return Serial.available();
}



uint8_t waitForValidMessage(String* sentMessage){
    // Wait for Message available in Buffer 
    while(Serial.available() == 0);
    String m = Serial.readString();
    // Check if Message is Part of registered Messages
    for(int i = 0; i<=sizeof(comm_dict)/sizeof(comm_dict[0]); i++){
        if(m.compareTo(comm_dict[i])){
            break;
        }
    }
    
   
    
}




