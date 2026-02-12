#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FatFS.h>
#include <FatFSUSB.h>
#include <stdint.h>
#include <VFS.h>

extern volatile bool updated;
extern volatile bool driveConnected;
extern volatile bool inPrinting;

bool FatFS_Setup();
bool FatFSUSB_Setup();

void unplug(uint32_t i);
void plug(uint32_t i);
bool mountable(uint32_t i);
bool sync_files();

#endif