#include "pc_link.h"
#include "weather.h"

static String rxLine;


void PC_Link_Init() {
    rxLine.reserve(256); // Prealloca spazio per la linea
}

void PC_Link_Update(){
    while(Serial.available()){
        char c = (char)Serial.read();
        if(c == '\n'){
            Weather_HandleLine(rxLine);
            rxLine = "";
        } else if(c != '\r') {
            rxLine += c;
            if(rxLine.length() >= 600) {
                rxLine = ""; // evita linee infinite
            }
        }
    }
}