#ifndef _4B940_H_
#define _4B940_H_

#include "global.h"

void Rom_InitDevice(s32);
void Rom_DmaRead(uintptr_t dev_addr, uintptr_t dram_addr, u32 size);

#endif // _4B940_H_
