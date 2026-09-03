#ifndef _FRAGMENT1_H_
#define _FRAGMENT1_H_

#include "global.h"
#include "src/gb_tower_audio.h"

void GbEmu_Init(unk_D_800AA660*);
void GbEmu_RunFrame(unk_D_800AA660*);
void GbEmu_AudioVideoTick(unk_D_800AA664*);
void func_81209078();
void GbEmu_InitContext(unk_D_8122B2C0*);
s32 osGbSetNextBuffer(void*, s32);

#endif /* _FRAGMENT1_H_ */
