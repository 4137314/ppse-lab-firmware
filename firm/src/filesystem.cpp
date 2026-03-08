#include "filesystem.h"

volatile bool updated = false;

volatile bool driveConnected = false;

volatile bool inPrinting = false;

bool FatFS_Setup() {
    inPrinting = true;
    if (!FatFS.begin()) {
        Serial.println("FatFS first mount failed. Formatting...");
        FatFS.format();
        if (!FatFS.begin()) {
            Serial.println("FATAL: FatFS format/mount failed!");
            while (1) delay(10);
        }
    }
    inPrinting = false;
    Serial.println("Fat filesystem started");
    VFS.root(FatFS);  // Enables POSIX functions for I/O
    return true;
}

bool FatFSUSB_Setup() {
    // Set up callbacks
    FatFSUSB.onUnplug(unplug);
    FatFSUSB.onPlug(plug);
    FatFSUSB.driveReady(mountable);

    if (!FatFSUSB.begin()) {
        Serial.println("FATAL failed to start USB Mass Storage");
        while (1) delay(10);
    }
    delay(3000);
    if (Serial)
        Serial.println("USB Mass Storage started");

    return true;
}

void unplug(uint32_t i) {
    (void)i;
    driveConnected = false;
    updated        = true;
}

void plug(uint32_t i) {
    (void)i;
    driveConnected = true;
    FatFS.end();
}

bool mountable(uint32_t i) {
    (void)i;
    return !inPrinting;
}

bool sync_files() {
    bool ret = true;

    if (updated && !driveConnected) {
        Serial.println("\n\nDisconnected, new file listing:");
        FatFS.end();
        ret     = FatFS_Setup();
        updated = false;
    }
    return ret;
}
