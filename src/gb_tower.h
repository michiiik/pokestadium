#ifndef _SRC_GB_TOWER_H_
#define _SRC_GB_TOWER_H_

#include "global.h"

s32 GbPak_VerifyPattern(s32 arg0, void* arg1);
s32 GbPak_DetectRam(s32 arg0, u8* arg1);
s32 GbTower_ProbePak(s32 arg0, u8* arg1, u8* arg2);
s32 GbTower_CheckPakRemoved(s32 arg0, u8 arg1);
void GbTower_PowerOffAllPaks(void);
s32 GbPak_IsPokemonCartridge(OSGbpakId* header);
s32 GbTower_ScanPaks(void);
s32 GbPak_IsCartOff(s32 arg0);
s32 GbPak_IsCartOn(s32 arg0);
s32 GbTower_ReinitPak(s32 arg0);
s32 GbTower_VerifyPakStillInserted(s32 arg0, void* arg1);
s32 GbPak_MbcRead(s32 arg0, void* arg1, u16 arg2, u16 arg3);
s32 GbPak_MbcWrite(s32 arg0, void* arg1, u16 arg2, u16 arg3);
s32 GbPak_RawRead(s32 arg0, u8* arg1, u16 arg2, u16 arg3);
s32 GbPak_RawWrite(s32 arg0, u8* arg1, u16 arg2, u16 arg3);
int GbPak_SelectRomBank(s32 arg0, s32 arg1);
s32 GbPak_ReadRom(s32 arg0, u8* arg1, s32 arg2, s32 arg3);
u8 GbTower_SetPollMask(u8 arg0);
void GbTower_PollPakConnection(void);

#endif /* _SRC_GB_TOWER_H_ */
