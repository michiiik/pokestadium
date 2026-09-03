#ifndef _4CBC0_H_
#define _4CBC0_H_

#include "global.h"

extern s32 D_80078FA8;
extern u32 D_80079014;

s32 Audio_PlayCategory2SoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategory2SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory3SoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategory3SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_ComputeIndexedVolume(u32 arg0, u32 arg1);
s32 Audio_PlayCategory4SoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategory4SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory5SoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategory5SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory6SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2);
void Audio_StopCategory6SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory7SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2);
void Audio_StopCategory7SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
void Audio_SeedRandom(s32 arg0, u32 arg1);
u32 Audio_NextRandom(void);
f32 Audio_RandomFloat(void);
s32 Audio_PlayCategory8SoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategory8SoundCommand(s32 arg0, s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory9SoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
void Audio_StopCategory9SoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategoryASoundCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_StopCategoryASoundCommand(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayPersistentSoundCommand(s32 arg0, u32 arg1);
s32 Audio_DispatchSoundBankCommand(s32 arg0, s32 arg1, s32 arg2);
void Audio_DispatchSoundBankStopCommand(s32 arg0, s32 arg1, s32 arg2);
s32 Audio_PlayDirectSoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);
s32 Audio_PlayCategory11SoundCommand(s32 arg0, s32 arg1, s32 arg2);
s32 Audio_PlayCategory12SoundCommand(s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2);

#endif // _4CBC0_H_
