#include"Filesystem.h"

volatile bool updated = false;
volatile bool driveConnected = false;
volatile bool inPrinting = false; // flag for FatFS Accessing


bool FatFS_Setup(){
    inPrinting = true;
    if (!FatFS.begin()) 
    {
        Serial.println("FatFS first mount failed. Formatting...");
        FatFS.format();
        if (!FatFS.begin()) {
            Serial.println("FATAL: FatFS format/mount failed!");
            while (1) delay(10);
        }
    }
    inPrinting = false;
    Serial.println("Fat filesystem started");
    VFS.root(FatFS); // Enables POSIX functions for I/O
    return true;
}



bool FatFSUSB_Setup(){
    // Set up callbacks
    FatFSUSB.onUnplug(unplug);
    FatFSUSB.onPlug(plug);
    FatFSUSB.driveReady(mountable);

    if (!FatFSUSB.begin())
    {
        Serial.println("FATAL failed to start USB Mass Storage");
        while (1) delay(10);
    }
    delay(2000); 
    Serial.println("USB Mass Storage started");

    return true;
}


// Called by FatFSUSB when the drive is released.  We note this, restart FatFS, and tell the main loop to rescan.
void unplug(uint32_t i) {
  (void) i;
  driveConnected = false;
  updated = true;
  //FatFS_Setup();
}


// Called by FatFSUSB when the drive is mounted by the PC.  Have to stop FatFS, since the drive data can change, note it, and continue.
void plug(uint32_t i) {
  (void) i;
  driveConnected = true;
  FatFS.end();
}


// Called by FatFSUSB to determine if it is safe to let the PC mount the USB drive.  If we're accessing the FS in any way, have any Files open, etc. then it's not safe to let the PC mount the drive.
bool mountable(uint32_t i) {
  (void) i;
  return !inPrinting;
}



bool sync_files() {
    bool ret = true;

    if (updated && !driveConnected) 
    {
        //inPrinting = true;
        Serial.println("\n\nDisconnected, new file listing:");
        FatFS.end();
        ret  = FatFS_Setup();
        updated = false;
        //inPrinting = false;
    }
    return ret;
}