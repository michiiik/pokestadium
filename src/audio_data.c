#include "audio_data.h"
#include "src/libnumus/player.h"
#include "src/rom_device.h"
#include "src/audio_cache.h"
#include "src/audio_category_fade.h"
#include "src/audio_commands.h"

u8* gSfxBankSpecial;

s32 Audio_LoadWaveTablesAndMusic(u8* arg0) {
    UNUSED s32 pad[2];
    u8* sp1C;

    Audio_FadeCategoriesAndStreams(1);
    gSfxBankSpecial = arg0;
    sp1C = gSfxBankSpecial + 0x4120;

    if (1) {}

    Rom_DmaRead(gSfxArchiveHeader->offset1, gMoveSoundTempBuffer, gSfxArchiveHeader->wave_tables_offset - gSfxArchiveHeader->offset1);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxBankSpecial, 0x4120);
    Rom_DmaRead(gSfxArchiveHeader->wave_tables_offset, sp1C, gSfxArchiveHeader->files[0] - gSfxArchiveHeader->wave_tables_offset);
    Audio_RelocateSoundBankWrapper(gSfxBankSpecial, sp1C);
    gAudioDMAFlags |= 0x80000000;
    Rom_DmaRead(gSfxArchiveHeader->files[0], gMoveSoundTempBuffer, gSfxArchiveHeader->files[1] - gSfxArchiveHeader->files[0]);
    Audio_CopyDataWithCacheSync(gMoveSoundTempBuffer, gSfxWaveTableBuffer, 0x98D8);
    gMusicHandle = Audio_PlayMusicSequence(gSfxBankSpecial, gSfxWaveTableBuffer);
    Audio_QueueVoiceVolumeScale(gMusicHandle, 0x70);

    return gMusicHandle;
}

void Audio_ClearWaveDataLoaded(void) {
    gAudioDMAFlags &= ~0x80000000;
}
