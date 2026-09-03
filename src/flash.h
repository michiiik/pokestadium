#ifndef _D470_H_
#define _D470_H_

#include "global.h"

OSPiHandle* Flash_Reinitialize(u8 latency, u8 pulse, u8 page_size, u8 rel_duration, u32 start);
OSPiHandle* Flash_Initialize(void);
void Flash_ReadStatus(u8* flash_status);
void Flash_ReadId(u32* flash_type, u32* flash_maker);
void Flash_ClearStatus(void);
s32 Flash_EraseChip(void);
s32 Flash_EraseSector(s32 arg0);
s32 Flash_StartWriteBuffer(OSIoMesg* mb, s32 priority, void* dramAddr, OSMesgQueue* mq);
s32 Flash_ProgramPage(s32 arg0);
s32 Flash_StartReadArray(OSIoMesg* mb, s32 priority, s32 devAddr, void* dramAddr, s32 n_pages, OSMesgQueue* mq);

#endif // _D470_H_
