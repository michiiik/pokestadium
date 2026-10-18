#ifndef _FRAGMENT1_H_
#define _FRAGMENT1_H_

#include "global.h"
#include "src/gb_tower_audio.h"

extern u8 D_102BA0_END[];

void GbEmu_Init(unk_D_800AA660*);
void GbEmu_RunFrame(unk_D_800AA660*);
void GbEmu_AudioVideoTick(unk_D_800AA664*);
s32 func_81209078(void);
void GbEmu_InitContext(unk_D_8122B2C0*);
s32 osGbSetNextBuffer(void*, s32);
void GbAudio_SetAlternateStreamId(s32);
u8 GbMem_ReadIoRegister(u16);
s32 GbCpu_Execute(unk_D_8122B2C0*, s32);
void GbCpu_InitOpcodeTables(unk_D_8122B2C0*, void*);
void func_81209374(s32, s16*);

#endif /* _FRAGMENT1_H_ */
