#ifndef PORT_AUDIO_H
#define PORT_AUDIO_H

// PORT: helpers for the audio engine's ROM data on a 64-bit little-endian host.
//
// The sound banks, songs, FX tables and archive headers are DMA'd from ROM as big-endian blobs whose pointer fields
// are 32-bit offsets the game relocated in place into absolute addresses. Here the headers are byte-swapped on load,
// offsets stay offsets inside a blob (the readers add the blob's address), and the parts the synthesizer reads through
// real pointers (wave tables) are rebuilt as host structures (port_audio.c).

#include "global.h"
#include <PR/libaudio.h>

static inline u32 PortA_BE32(const void* p) {
    const u8* b = (const u8*)p;
    return ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | b[3];
}
static inline u16 PortA_BE16(const void* p) {
    const u8* b = (const u8*)p;
    return (u16)((b[0] << 8) | b[1]);
}
void PortA_Swap32(void* p, u32 count); // big-endian u32 words -> host order, in place
void PortA_Swap16(void* p, u32 count);

// 32-bit ids for pointers that the game stores in 32-bit fields (the player's command queue)
u32 PortA_PtrId(const void* p);
void* PortA_IdPtr(u32 id);

// Size of an ALSeqFile with n entries in host layout (ALSeqData holds a pointer: 16 bytes instead of 8).
#define PORT_SEQFILE_BYTES(n) (sizeof(ALSeqFile) + ((n) > 1 ? (n) - 1 : 0) * sizeof(ALSeqData))
// ... and as stored in ROM (s16 revision, s16 count, then {u32 offset, s32 len} per entry)
#define PORT_SEQFILE_ROM_BYTES(n) (4 + (n) * 8)

// Sound bank ("N64 PtrTablesV2"): the host-side view built by PortA_RelocateSoundBank.
typedef struct PortSoundBank {
    s32 count;
    u8* basenote;            // count bytes in the blob
    f32* detune;             // count floats in the blob (converted in place by the game)
    ALWaveTable** wave_list; // host wave tables (loops and ADPCM books stay in the blob, byte-swapped)
} PortSoundBank;
PortSoundBank* PortA_RelocateSoundBank(void* blob, uintptr_t waveRomBase); // NULL when already relocated
PortSoundBank* PortA_Bank(const void* blob);

// Blocks the calling thread for one frame (for loops that spin on the sound engine's state).
void PortA_SpinWait(void);

#endif
