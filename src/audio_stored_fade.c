#include "audio_stored_fade.h"
#include "libnumus/player.h"
#include "audio_commands.h"
#include "audio_cache.h"
#include "rom_device.h"
#include "cry.h"

// .data
u8 D_80078A10[3] = { 0xFF, 0xFF, 0xFF };
u32 D_80078A14 = 0;
s32 D_80078A18 = 0;
u8 D_80078A1C[4] = { 0 };
u8 D_80078A20[16] = { 0 };

#ifdef NON_MATCHING
void func_80048060(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    u8 sp33;
    s32 var_s0;
    s32 var_a3;
    s32 var_t0;
    s32 temp_v1;
    s32 var_a0;
    s32 var_a2;
    u32 temp_a3;
    s32 var_s0_2;
    u32 *var_v1;
    ALSeqData *temp_v1_3;
    s32 var_a2_2;
    s32 var_a0_2;
    s32 var_a2_3;
    s32 var_s0_3;
    u8 **var_v1_2;
    u8 **var_a1;
    u8 *entry_ptr;
    bank_remap_descriptor_t *temp_a1;

    var_s0 = arg2;
    sp33 = 0;
    D_80078A20[0] = 0;
    Audio_QueueFadeSoundCommand(D_80078A18, 0xA);

    if (var_s0 == 0xC8) {
        sp33 = 1;
    } else if ((var_s0 == 0) || ((u32)var_s0 >= 0x98)) {
        return;
    }

    if ((var_s0 == 0) || ((u32)arg1 >= 0xA6)) {
        return;
    }

    var_t0 = arg3 & 0xFF;
    temp_v1 = (arg3 >> 8) & 0xFF;
    if (((var_t0 == 0xA) || (var_t0 == 0xB)) && ((temp_v1 == 0x10) || (temp_v1 == 0x20))) {
        return;
    }

    var_a3 = 0;
    while (D_80078A10[var_a3] != var_s0) {
        var_a3++;
        if (var_a3 == 3) {
            temp_a3 = D_80078A14 % 3;
            D_80078A14++;
            D_80078A10[temp_a3] = (u8)var_s0;

            if (sp33 != 0) {
                var_s0 = 0x97;
            }
            var_s0_2 = var_s0 - 1;
            var_v1 = (u32 *)D_800FC6FC;
            if ((u32)var_s0_2 < (D_800FC6FC->num_files - 1)) {
                var_a0 = var_v1[3 + var_s0_2];
                var_a2 = var_v1[3 + var_s0_2 + 1] - var_a0;
            } else {
                var_a0 = var_v1[3 + var_s0_2];
                var_a2 = (s32)D_800FC6E8->seqArray[3].offset - var_a0;
            }
            Rom_DmaRead(var_a0, (u32)D_800FC6DC, var_a2);
            Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC6B0[temp_a3], 0x3E8);
            Audio_RelocateSoundBankTable((bank_remap_descriptor_t *)D_800FC6B0[temp_a3]);

            if (sp33 != 0) {
                var_s0_2 = 0x97;
            }
            temp_v1_3 = &D_800FC700->seqArray[var_s0_2];
            var_a2_2 = (s32)temp_v1_3[1].offset;
            if (var_a2_2 & 1) {
                var_a2_2 += 1;
            }
            var_a0_2 = (s32)temp_v1_3->offset;
            Rom_DmaRead(var_a0_2, (u32)D_800FC6DC, var_a2_2 - var_a0_2);
            Audio_CopyDataWithCacheSync(D_800FC6DC, D_800FC6C0[temp_a3], 0x44C);
            var_a3 = temp_a3;
            break;
        }
    }

    if ((u32)var_t0 >= 0x11) {
        return;
    }

    switch (var_t0) {
        case 0:
        case 2:
        case 3:
            var_a2_3 = 0;
            var_v1_2 = &D_800FC6B0[var_a3];
            var_a1 = &D_800FC6C0[var_a3];
            goto block_35;
        case 4:
            var_a2_3 = 2;
            var_v1_2 = &D_800FC6B0[var_a3];
            var_a1 = &D_800FC6C0[var_a3];
            goto block_35;
        case 1:
            var_a2_3 = 4;
            var_v1_2 = &D_800FC6B0[var_a3];
            var_a1 = &D_800FC6C0[var_a3];
            goto block_35;
        case 11:
            var_a2_3 = 0;
            var_v1_2 = &D_800FC6B0[var_a3];
            var_a1 = &D_800FC6C0[var_a3];
            goto block_35;
        case 16:
            var_s0_3 = arg1;
            var_v1_2 = &D_800FC6B0[var_a3];
            goto block_38;
        default:
            return;
    }

block_35:
    if (var_t0 == 0xB) {
        arg1 = 0;
    }
    entry_ptr = *var_a1 + 0x10 + (arg1 * 6) + var_a2_3;
    var_s0_3 = entry_ptr[0];
    D_80078A1C[0] = entry_ptr[1];

block_38:
    temp_a1 = (bank_remap_descriptor_t *)*var_v1_2;
    if ((sp33 == 0) && ((u32)var_s0_3 >= temp_a1->num_entries)) {
        return;
    }
    if (sp33 != 0) {
        if ((arg2 == 0x2D) || (arg2 == 0x2E)) {
            D_80078A20[0] = 1;
        }
        if (var_s0_3 != 0) {
            Cry_Play((u8)var_s0_3, 0xA);
        }
        return;
    }
    D_80078A18 = Audio_PlaySoundEffect(D_800FC6AC, (fx_t *)temp_a1, var_s0_3, 0x80, 0x80, -1);
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/audio_stored_fade/func_80048060.s")
#endif

void Audio_FadeStoredSound(void) {
  if (D_80078A1C[0] != 0) {
      Audio_QueueFadeSoundCommand(D_80078A18, (s32)D_80078A1C[0]);
  }
}
