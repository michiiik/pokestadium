#ifndef _4BA90_H_
#define _4BA90_H_

#include "global.h"

void Audio_SetChannelVolume(s32 arg0, u32 arg1);
void Audio_SetMonoOutput(s32 arg0);
void Audio_SetPauseMixMode(u32 arg0);
void Audio_BeginVolumeTransition(u32 arg0);
void Audio_ResetVolumeTransition(void);

#endif // _4BA90_H_
