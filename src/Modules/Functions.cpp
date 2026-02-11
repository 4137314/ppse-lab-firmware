#include "Functions.h"

bool init_All(){
    bool status=true;

//////////////// Serial over USB //////////////////
    Serial.begin(9600);

    int i=0;
    while (!Serial && i<SERIAL_MAX_SETUP_TIME){
        delay(1);
        ++i;
    }

    if (i==1000)
    {
        Serial.println("ERROR: Serial over USB setup timed out");
        exit(EXIT_FAILURE);
    }

    Serial.println("<<<< START of hardware initialization >>>>");
    Serial.println("Serial is up!");

//////////////// TEMP SENSOR /////////////////////    
    Serial.print("Initializing temperture sensor...");
    TempInit();

//////////////// OnBoard LEDs ////////////////////
    ledsInit();

//////////////// BUTTONS /////////////////////////  
    buttonsInit();

///////////////// BUZZER /////////////////////////
    //buzzerInit(BUZZER);

//////////////// OLED DISPLAY ////////////////////
    status = status && displayInit();

//////////////// GPS MODULE //////////////////////
    status = status && gpsInit();

    Serial.println("<<<< END of hardware initialization >>>>");
    return status;
}



// Monta il file system
void littleFS_Setup(){
    if (!LittleFS.begin()) 
    {        
        #if DEBUG == 1
        Serial.print("WARNING in littleFS_Setup: mount failed, trying to format and remount...");
        #endif 

        // try formatting
        if(!LittleFS.format()) 
        {
            Serial.println("");
            Serial.println("ERROR in littleFS_Setup: cannot format the filesystem");
            exit(EXIT_FAILURE);
        }
        
        // try to remount
        if (!LittleFS.begin()) 
        {
            Serial.println("");
            Serial.println("ERROR in littleFS_Setup: cannot format the filesystem");
            exit(EXIT_FAILURE);
        }
        #if DEBUG == 1
        Serial.println("OK");
        #endif
    }
    return;
}


bool flash_write(const char* path, const char* content, const char* mode = "w") {
    // Apri il file
    File file = LittleFS.open(path, mode);
    
    if (!file) {
        Serial.printf("Errore: Impossibile aprire %s in modalità %s\n", path, mode);
        return false;
    }

    // Scrivi il contenuto
    size_t bytesWritten = file.print(content);
    
    // Chiudi sempre il file per salvare i dati fisicamente
    file.close();

    if (bytesWritten > 0) {
        #if DEBUG == 1
        Serial.printf("Scrittura su %s completata (%d bytes)\n", path, bytesWritten);
        #endif
        return true;
    } else {
        Serial.println("Errore durante la scrittura: 0 bytes scritti.");
        return false;
    }
}
