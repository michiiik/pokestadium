#ifndef _4A3E0_H_
#define _4A3E0_H_

#include "global.h"

void GbAudio_GenerateSamples(s16*, s32, u32, f32);
void GbAudio_QueueRegisterWrite(u16, u8, u16);
u8 GbAudio_GetRegisterValue(u16);
void func_800498A8(u32, u8, u32);

#endif // _4A3E0_H_
