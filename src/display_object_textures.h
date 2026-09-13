#ifndef _19840_H_
#define _19840_H_

#include "global.h"
#include "src/jpeg_decoder.h"
#include "src/matrix.h"
#include "src/model_renderer.h"
#include "src/game_state.h"

typedef struct unk_func_80019600 {
    /* 0x00 */ s16 jobType; // PokeIcon_ThreadMain's dispatch (1=LoadModelArchive, 2=LoadMultipleModels)
    /* 0x02 */ s16 archiveIndex; // PokeIcon_LoadModelArchive's BinArchive_LoadFileToPool index
    /* 0x04 */ OSMesgQueue* queue;
    /* 0x08 */ void* result; // single Fragment* result (jobType 1) or an s32 result array (jobType 2)
    /* 0x0C */ void* unk_0C; // PokeIcon_LoadMultipleModels: s16* index list, terminated by -1
    /* 0x10 */ MainPoolState* pool;
    /* 0x14 */ s32 size;
    /* 0x18 */ arg1_func_80010CA8 unk_18; // Fragment_ProcessGeoLayout's color-adjust arg
} unk_func_80019600; // size >= 0x1C

typedef struct sp18_func_800198E4 {
    /* 0x00 */ s16 jobType; // PokeIcon_ApplyLoadResult's dispatch (1=frame load, 2=background load)
    /* 0x02 */ u16 archiveIndex; // PokeIcon_RequestFrameLoad: arg1 - 1
    /* 0x04 */ s32 replyQueue; // OSMesgQueue*; the requester's unk_30 (frame) or unk_50 (background) queue
    /* 0x08 */ s32 result; // load result, applied back onto the requester by PokeIcon_ApplyLoadResult
    /* 0x0C */ s32 startAddr; // PokeIcon_RequestBackgroundLoad's arg1; 0 for frame loads
    /* 0x10 */ s32 poolPtr; // destination buffer: requester's per-variant model buffer or background-load buffer
    /* 0x14 */ s32 poolSize; // 0x3C000 (frame) or 0x18000 (background)
    /* 0x18 */ arg1_func_80010CA8 unk_18; // color-adjust arg, threaded through to the result
} sp18_func_800198E4; // size = 0x1C

typedef struct unk_D_800ABE10_A04 {
    /* 0x00 */ BinArchive* modelArchive; // PokeIcon_OpenModelArchives: single-model archive (0x920000)
    /* 0x04 */ s32 multiModelArchive; // mistyped s32 holding a BinArchive* (0x8CC000)
    /* 0x08 */ s32 scratchBuffer; // mistyped s32 holding a main_pool_alloc(0x28000) pointer
    /* 0x0C */ s32 capturedSceneNode; // PokeIcon_CaptureGeoNode; mistyped s32 holding a GraphNode*, shared render scene's display object
    /* 0x10 */ BinArchive* textureArchive; // PokeIcon_OpenModelArchive (0x820000); model texture source for PokeIcon_LoadModelTexture
} unk_D_800ABE10_A04; // size >= 0x14

typedef struct unk_D_800ABE10 {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char threadStack[0x800];
    /* 0x9B0 */ OSMesg requestMsgBuf[8]; // backs requestQueue; also doubles as the thread's stack-top pointer in osCreateThread
    /* 0x9D0 */ OSMesgQueue requestQueue;
    /* 0x9E8 */ OSMesg doneMsgBuf;
    /* 0x9EC */ OSMesgQueue doneQueue;
    /* 0xA04 */ unk_D_800ABE10_A04 archives;
} unk_D_800ABE10; // size >= 0xA18

extern unk_D_800ABE10 D_800ABE10;

void DisplayObject_AdjustTextureColors(unk_D_86002F34* arg0, arg1_func_80010CA8 arg1);
Fragment* Jpeg_AllocAndDecodeFragment(MemoryBlock* arg0, PRESJPEG* arg1, PRESJPEG* arg2);
Fragment* Yay0_AllocAndDecompressFragment(MainPoolState* arg0, PERSSZP* arg1, PERSSZP* arg2);
Fragment* Asset_AllocAndCopyFragment(MainPoolState* arg0, PERSSZP* arg1, s32 arg2, u32 arg3);
Fragment* Fragment_LoadFromRomRange(MainPoolState* arg0, s32 start, s32 end, PERSSZP* arg3, s32 arg4);
Fragment* Fragment_LoadFromLbaRange(MainPoolState* arg0, unk_func_800041C0* arg1, PERSSZP* arg2, u32 arg3);
void Fragment_RelocateAndCommit(MainPoolState* arg0, u32 arg1, Fragment* arg2);
Fragment* BinArchive_LoadFileFromRomRange(MainPoolState* arg0, BinArchive* arg1, BinArchiveFile* arg2, u32 arg3, s32 arg4);
Fragment* BinArchive_LoadFileFromLbaRange(MainPoolState* arg0, BinArchive* arg1, BinArchiveFile* arg2, u32 arg3, s32 arg4);
Fragment* BinArchive_LoadFileToPool(MainPoolState* arg0, BinArchive* arg1, s32 arg2, u32 arg3, s32 arg4);
s32 Fragment_ProcessGeoLayout(MemoryBlock* arg0, Fragment* arg1, arg1_func_80010CA8 arg2);
void Fragment_ProcessGeoLayoutList(MemoryBlock* arg0, arg1_func_80019420* arg1);
void PokeIcon_LoadModelArchive(unk_func_80019600* arg0);
void PokeIcon_LoadMultipleModels(unk_func_80019600* arg0);
void PokeIcon_ThreadMain(UNUSED void* arg0);
void PokeIcon_StartThread(void);
unk_D_86002F58_004_000_010* PokeIcon_AllocFramebuffers(u32 arg0);
void PokeIcon_OpenModelArchives(void);
s32 PokeIcon_RequestFrameLoad(unk_D_86002F58_004_000_010* arg0, u16 arg1, arg1_func_80010CA8 arg2);
s32 PokeIcon_RequestFrameLoadWithVariant(unk_D_86002F58_004_000_010* arg0, u16 arg1, arg1_func_80010CA8 arg2, s16 arg3);
s32 PokeIcon_RequestBackgroundLoad(unk_D_86002F58_004_000_010* arg0, s32 arg1, s32 arg2);
s32 PokeIcon_ApplyLoadResult(unk_D_86002F58_004_000_010* arg0, sp18_func_800198E4* arg1);
s32 PokeIcon_PollFrameLoad(unk_D_86002F58_004_000_010* arg0);
s32 PokeIcon_PollBackgroundLoad(unk_D_86002F58_004_000_010* arg0);
s32 PokeIcon_WaitFrameLoad(unk_D_86002F58_004_000_010* arg0);
s32 PokeIcon_WaitBackgroundLoad(unk_D_86002F58_004_000_010* arg0);
unk_D_86002F30* Model_LoadByArchiveIndex(s32 arg0);
unk_D_86002F30* Model_LoadForPokemon(BattleMon* arg0);
s32 Model_LoadWithScale(s32 arg0, s32 arg1);
arg1_func_80019420* BattleAnim_LoadResourceTable(s32 arg0);
Fragment* Fragment_LoadAndRelocateFromRomRange(MainPoolState* arg0, u32 arg1, s32 arg2, s32 arg3, PERSSZP* arg4, s32 arg5);


#endif // _19840_H_
