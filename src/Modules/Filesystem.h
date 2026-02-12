#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <FatFS.h>
#include <FatFSUSB.h>
#include <stdint.h>
#include <VFS.h>

bool FatFS_Setup();

bool FatFSUSB_Setup();

void unplug(uint32_t i);
void plug(uint32_t i);
bool mountable(uint32_t i);

#endif