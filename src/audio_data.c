#include "audio_data.h"
#include "src/libnumus/player.h"
#include "src/rom_device.h"
#include "src/audio_cache.h"
#include "src/audio_category_fade.h"
#include "src/audio_commands.h"

u8* D_800FF9C0;

s32 Audio_LoadWaveTablesAndMusic(u8* arg0) {
    UNUSED s32 pad[2];
    u8* sp1C;

    Audio_FadeCategoriesAndStreams(1);
    D_800FF9C0 = arg0;
    sp1C = D_800FF9C0 + 0x4120;

    if (1) {}

    Rom_DmaRead(D_800FC6E4->offset1, D_800FC6DC, D_800FC6E4->wave_tables_offset - D_800FC6E4->offset1);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FF9C0, 0x4120);
    Rom_DmaRead(D_800FC6E4->wave_tables_offset, sp1C, D_800FC6E4->files[0] - D_800FC6E4->wave_tables_offset);
    Audio_RelocateSoundBankWrapper(D_800FF9C0, sp1C);
    D_800FC820 |= 0x80000000;
    Rom_DmaRead(D_800FC6E4->files[0], D_800FC6DC, D_800FC6E4->files[1] - D_800FC6E4->files[0]);
    Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC680, 0x98D8);
    D_80078E70 = Audio_PlayMusicSequence(D_800FF9C0, D_800FC680);
    Audio_QueueVoiceVolumeScale(D_80078E70, 0x70);

    return D_80078E70;
}

void Audio_ClearWaveDataLoaded(void) {
    D_800FC820 &= ~0x80000000;
}
