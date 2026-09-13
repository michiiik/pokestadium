#include "pokemon_stats.h"
#include "src/gb_data.h"
#include "src/save_data.h"
#include "src/text_system.h"
#include "src/gb_tower.h"
#include "src/hal_libc.h"
#include "src/util.h"

typedef struct unk_func_800228F0_sp24 {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ char unk_04[0x10];
    /* 0x14 */ char unk_14[0x8];
} unk_func_800228F0_sp24; // size >= 0x1C

extern GbSavePort D_800AC910[4];
extern u32 D_800ACA70;
extern u8 D_800ACA74;
extern u8 D_800ACA75;
extern GbSaveMainData D_800ACA78;

extern u8 gDexToInternalIndex[];
extern u8 gInternalIndexToDex[];

extern char D_800AC890[][0x20];

u16 Pokemon_LevelFromExp(s32 arg0, u32 arg1) {
    s32 i;

    for (i = 1; i < 100; i++) {
        if (arg1 < Pokemon_ExpForLevel(arg0, i + 1)) {
            break;
        }
    }

    return i;
}

void Pokemon_PrepareBattleMon(BattleMon* arg0) {
    s32 i;
    s32 temp_a3;
    s32 var_v0;
    s32 idx;

    Pokemon_RecalcStats(arg0);
    arg0->status = 0;
    arg0->currentHP = arg0->maxHP;

    for (i = 0; i < 4; i++) {
        if (arg0->moves[i] != 0) {
            temp_a3 = (arg0->pp[i] >> 6) & 3;
            var_v0 = gMoveData[arg0->moves[i] - 1].basePP;

            if (var_v0 >= 0x28) {
                var_v0 = var_v0 + (temp_a3 * 7);
            } else {
                var_v0 = var_v0 + ((var_v0 / 5) * temp_a3);
            }

            arg0->pp[i] = (temp_a3 << 6) + var_v0;
        }
    }
}

void Pokemon_SetDisplayNameFromOt(BattleMon* arg0) {
    UNUSED s32 pad;
    char sp18[11];

    Text_TranscodeName(&sp18, &arg0->otName);
    _bcopy(&sp18, arg0->otNameEncoded, 0xB);
}

s32 VictoryPalace_LoadSpeciesRecord(unk_func_800228F0* arg0, u16 arg1) {
    unk_func_800228F0_sp24 sp24;
    s32 var_v1;

    var_v1 = 0;
    if ((Deck_ReadSaveEntry(0x13, 0, arg1 - 1, &sp24) != 0) && (sp24.unk_00 != 0)) {
        arg0->speciesId = sp24.unk_00;
        arg0->level = sp24.unk_01;
        arg0->otId = sp24.unk_02;
        Text_UntranscodeFixedName(arg0->nickname, &sp24.unk_04[0]);
        Text_UntranscodeFixedName(arg0->otName, &sp24.unk_04[0xB]);
        var_v1 = 1;
    }
    return var_v1;
}

void VictoryPalace_SaveSpeciesRecord(BattleMon* arg0, s16 arg1, u8 arg2) {
    unk_D_800AE4E8_004_2_0DC0_002 sp24;

    if (arg2) {
        sp24.speciesId = arg1 + 0x80;
    } else {
        sp24.speciesId = arg1;
    }
    sp24.level = arg0->level;
    sp24.otId = arg0->otId;
    Text_TranscodeName(sp24.nickname, arg0->nickname);
    Text_TranscodeName(sp24.otName, arg0->otName);
    Save_WriteTypedRecord(0x13, 0, arg0->species.dexId - 1, &sp24);
}

s32 GbSave_PlayerIdentityMatches(u16* arg0, GbSavePlayerIdentity* arg1) {
    return !bcmp(arg0, arg1, 0xD);
}

u8 Move_GetIdFromTableIndex(s32 arg0) {
    extern u8 D_80073230[];

    return D_80073230[arg0];
}

u8 Species_ToInternalIndex(s32 arg0) {
    if ((arg0 <= 0) || (arg0 > 151)) {
        return 0;
    }

    return gDexToInternalIndex[arg0];
}

s32 Species_FromInternalIndex(s32 arg0) {
    if ((arg0 <= 0) || (arg0 > 190)) {
        return 152;
    }

    return gInternalIndexToDex[arg0];
}

u8 func_80022A88(s32 arg0) {
    extern u8 D_800733BF[];

    return D_800733BF[arg0];
}

u8 func_80022A98(s32 arg0) {
    extern u8 D_80073457[];

    return D_80073457[arg0];
}

s32 func_80022AA8(s32 arg0) {
    extern u8 D_800734EF[];

    if (arg0 == 0) {
        return 0;
    }
    return D_800734EF[arg0] + 1;
}

s32 func_80022ACC(s32 arg0) {
    extern u8 D_80073597[];

    if (arg0 == 0) {
        return 0;
    }

    return D_80073597[arg0] + 1;
}

void Deck_InitializeSaveBackend(DeckHandle* arg0) {
    if (arg0->mode == 1) {
        arg0->dirty |= 1;
        Deck_SetSaveEntryCount(arg0->deckType, arg0->slot, 0);
    }

    arg0->recordSize = Deck_GetSaveRecordSize(arg0->deckType);
    arg0->capacity = Deck_GetSaveRecordCapacity(arg0->deckType);
    arg0->count = Deck_GetSaveEntryCount(arg0->deckType, arg0->slot);

    if (arg0->mode == 2) {
        arg0->cursor = arg0->count;
    } else {
        arg0->cursor = 0;
    }
}

void Deck_InitializeBoxBackend(DeckHandle* arg0) {
    if (arg0->mode == 1) {
        arg0->dirty |= 1;
        Deck_DeleteEntry(arg0->deckType, arg0->port, arg0->slot, 0);
    }

    arg0->recordSize = Deck_GetEntrySize(arg0->deckType);
    arg0->capacity = Deck_GetCapacity(arg0->deckType);
    arg0->count = Deck_GetCount(arg0->deckType, arg0->port, arg0->slot);

    if (arg0->mode == 2) {
        arg0->cursor = arg0->count;
    } else {
        arg0->cursor = 0;
    }
}

DeckHandle* Deck_Open(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    DeckHandle* sp1C;

    sp1C = Util_Malloc(sizeof(DeckHandle));
    if (sp1C != NULL) {
        sp1C->mode = arg3;
        sp1C->deckType = arg0;
        sp1C->port = arg1;
        sp1C->slot = arg2;
        sp1C->dirty = 0;

        switch (arg0 & 0xF0) {
            case 16:
                Deck_InitializeSaveBackend(sp1C);
                break;

            case 32:
                Deck_InitializeBoxBackend(sp1C);
                break;
        }
    }

    return sp1C;
}

DeckHandle* Deck_OpenAndSetName(s32 arg0, s32 arg1, s32 arg2, s32 arg3, char* arg4, u16 arg5) {
    DeckHandle* sp24;
    DeckHandle* temp_v0;
    DeckHandle* var_v1;

    sp24 = Deck_Open(arg0, arg1, arg2, 1);
    if ((sp24 != NULL) && (arg0 & 0xF0)) {
        Deck_SetSaveEntryTrainerIdAndName(arg0, arg2, arg3, arg4, arg5);
    }
    return sp24;
}

s32 Deck_CloseAndFlushBox(DeckHandle* arg0) {
    s32 sp1C = 0;

    if ((arg0->deckType & 0x10) && (arg0->dirty & 1)) {
        Save_InvalidateTypedRecord(arg0->deckType, arg0->slot);
        sp1C = 1;
    }

    Util_Free(arg0);

    return sp1C;
}

s32 Deck_CloseAndFlush(DeckHandle* arg0) {
    s32 sp1C = 0;

    if ((arg0->deckType & 0x10) && (arg0->dirty & 1)) {
        Save_CommitTypedRecord(arg0->deckType, arg0->slot);
        sp1C = 1;
    }

    Util_Free(arg0);

    return sp1C;
}

s32 Deck_SetCursor(DeckHandle* arg0, s32 arg1) {
    s32 var_v1;

    var_v1 = 0;
    if (arg0->count >= arg1) {
        arg0->cursor = arg1;
        var_v1 = 1;
    }
    return var_v1;
}

s32 Deck_ReadEntries(u8* arg0, s32 arg1, DeckHandle* arg2) {
    s32 var_s3;
    s32 var_s4;
    s32 var_v1;
    s32 tmp;

    var_s4 = 0;
    var_s3 = arg1 - 1;
    tmp = arg1 > 0;

    if (tmp) {}

    if (tmp != 0) {
        for (; arg2->cursor < arg2->count;) {
            var_v1 = 0;
            switch (arg2->deckType & 0xF0) {
                case 16:
                    var_v1 = Deck_ReadSaveEntry(arg2->deckType, arg2->slot, arg2->cursor, arg0);
                    break;

                case 32:
                    var_v1 = Deck_ReadEntry(arg2->deckType, arg2->port, arg2->slot, arg2->cursor, arg0);
                    break;
            }

            tmp = var_s3 > 0;
            if (var_v1 == 0) {
                break;
            }

            arg2->cursor++;
            var_s3 -= 1;
            var_s4 += 1;
            arg0 += arg2->recordSize;

            if (!tmp) {
                break;
            }
        }
    }
    return var_s4;
}

s32 Deck_WriteEntries(u8* arg0, s32 arg1, DeckHandle* arg2) {
    s32 var_s2;
    s32 var_s4;
    s32 var_s5;
    s32 tmp;

    var_s5 = 0;
    var_s4 = arg1 - 1;
    tmp = arg1 > 0;

    if (tmp) {}

    if (tmp) {
        for (; arg2->cursor < arg2->capacity;) {
            var_s2 = 0;
            switch (arg2->deckType & 0xF0) {
                case 16:
                    var_s2 = Save_WriteTypedRecord(arg2->deckType, arg2->slot, arg2->cursor, arg0);
                    arg2->count = Deck_GetSaveEntryCount(arg2->deckType, arg2->slot);
                    break;

                case 32:
                    var_s2 = Deck_WriteEntry(arg2->deckType, arg2->port, arg2->slot, arg2->cursor, arg0);
                    arg2->count = Deck_GetCount(arg2->deckType, arg2->port, arg2->slot);
                    break;
            }

            tmp = var_s4 > 0;
            if (var_s2 == 0) {
                break;
            }

            var_s4 -= 1;
            arg2->cursor++;
            var_s5 += 1;
            arg0 += arg2->recordSize;
            arg2->dirty |= 1;

            if (!tmp) {
                break;
            }
        }
    }

    return var_s5;
}

void Text_LoadPlayerLabels(void) {
    char** sp1C = Text_GetStringTable(0x11);

    HAL_Strcpy(D_800AC890[0], Text_GetString(NULL, 0, sp1C, 0));
    HAL_Strcpy(D_800AC890[1], Text_GetString(NULL, 0, sp1C, 1));
    HAL_Strcpy(D_800AC890[2], Text_GetString(NULL, 0, sp1C, 2));
    HAL_Strcpy(D_800AC890[3], Text_GetString(NULL, 0, sp1C, 3));
}

char* Text_GetPlayerLabel(s32 arg0) {
    return D_800AC890[arg0];
}

s32 GbSave_HasPlayerNameMarker(u8* arg0) {
    s32 i;

    for (i = 0; i < 11; i++) {
        if (*arg0++ == 'P') {
            return 1;
        }
    }

    return 0;
}

s32 GbSave_VerifyChecksum(u8* arg0, u8* arg1) {
    s32 sp1C = 0;
    s32 sp18 = 0;

    if (GbSave_HasPlayerNameMarker(arg0) != 0) {
        while (arg1 >= arg0) {
            sp18 += *arg0++;
        }
        sp1C = (sp18 & 0xFF) == 0xFF;
    }

    return sp1C;
}

void GbSave_ComputeChecksum(u8* arg0, u8* arg1) {
    s32 var_v0 = 0;

    while (arg0 < arg1) {
        var_v0 += *arg0++;
    }

    *arg1 = 0xFF - (var_v0 & 0xFF);
}

s32 GbSave_VerifyWriteback(u8* arg0, s32 arg1) {
    u8* var_v0 = D_800ACA78.unk_000;

    while (arg1-- > 0) {
        if (*arg0++ != *var_v0++) {
            return 0;
        }
    }

    return 1;
}

s8 GbCart_IdentifyGame(s32 arg0) {
    // Only part of the rom header,
    // osGbpakReadId reads from 0x100 but this reads from 0x120
    typedef struct RomHeader {
        /* 0x00 */ char nintendo_chr[0x14];
        /* 0x14 */ char game_title[16];
        /* 0x24 */ u16 company_code;
        /* 0x26 */ u8 body_code;
        /* 0x27 */ u8 cart_type;
        /* 0x28 */ u8 rom_size;
        /* 0x29 */ u8 ram_size;
        /* 0x2A */ s8 country_code;
        /* 0x2B */ u8 fixed2;
        /* 0x2C */ u8 version;
        /* 0x2D */ u8 isum;
        /* 0x2E */ u16 sum;
        /* 0x30 */ char unk30[0x30];
    } RomHeader; // size = 0x60

    RomHeader sp28;
    s8 var_s0 = POKEMON_ROM_NONE;

    if ((GbPak_ReadRom(arg0, &sp28, 0x120, sizeof(RomHeader)) == 0) && (sp28.country_code == 1)) {
        sp28.game_title[7] = '\x0';
        sp28.game_title[16] = '\x0';

        if (HAL_Strcmp(&sp28.game_title[0], "POKEMON") == 0) {
            if (HAL_Strcmp(&sp28.game_title[8], "RED") == 0) {
                var_s0 = POKEMON_ROM_RED;
            } else if (HAL_Strcmp(&sp28.game_title[8], "GREEN") == 0) {
                var_s0 = POKEMON_ROM_GREEN;
            } else if (HAL_Strcmp(&sp28.game_title[8], "BLUE") == 0) {
                var_s0 = POKEMON_ROM_BLUE;
            } else if (HAL_Strcmp(&sp28.game_title[8], "YELLOW") == 0) {
                var_s0 = POKEMON_ROM_YELLOW;
            }
        }
    }

    return var_s0;
}

s32 GbSave_CheckMainDataUnchanged(s32 arg0) {
    UNUSED s32 pad;
    s32 sp18 = 0;

    if ((GbPak_MbcRead(arg0, &D_800ACA78, 0x2580, sizeof(GbSaveMainData)) == 0) &&
        (GbSave_VerifyChecksum(&D_800ACA78.playerName, &D_800ACA78.checksumBlock.checksum) != 0)) {
        sp18 = D_800ACA78.checksumBlock.checksum == D_800AC910[arg0].mainDataChecksum;
    }
    return sp18;
}

s32 GbSave_ReadMainData(s32 arg0) {
    s32 i;
    s32 sp30;
    GbSaveMainData* temp_s4 = D_800AC910[arg0].mainData;

    sp30 = NULL;

    if (GbPak_MbcRead(arg0, temp_s4, 0x2580, sizeof(GbSaveMainData)) == 0) {
        for (i = 0; i < 3; i++) {
            sp30 = GbSave_VerifyChecksum(&temp_s4->playerName, &temp_s4->checksumBlock.checksum);
            if (sp30 != 0) {
                if (D_800AC910[arg0].flags & 0x20) {
                    D_800AC910[arg0].mainDataChecksum = temp_s4->checksumBlock.checksum;
                    D_800AC910[arg0].flags = D_800AC910[arg0].flags & ~0x20;
                } else {
                    sp30 = D_800AC910[arg0].mainDataChecksum == temp_s4->checksumBlock.checksum;
                }
                return sp30;
            }
        }
    }

    return sp30;
}

s32 GbSave_WriteMainData(s32 arg0) {
    s32 i;
    GbSavePort* temp_s3;
    GbSaveMainData* temp_s1 = D_800AC910[arg0].mainData;

    GbSave_ComputeChecksum(temp_s1->playerName, &temp_s1->checksumBlock.checksum);
    D_800AC910[arg0].mainDataChecksum = temp_s1->checksumBlock.checksum;

    for (i = 0; i < 4; i++) {
        if ((GbPak_MbcWrite(arg0, temp_s1, 0x2580, sizeof(GbSaveMainData)) == 0) &&
            (GbPak_MbcRead(arg0, &D_800ACA78, 0x2580, sizeof(GbSaveMainData)) == 0) &&
            (GbSave_VerifyWriteback(temp_s1, sizeof(GbSaveMainData)) != 0)) {
            return 1;
        }
    }
    return 0;
}

s32 GbSave_InvalidateMainDataChecksum(s32 arg0) {
    GbSaveMainData* temp_s3 = D_800AC910[arg0].mainData;
    s32 i;

    GbSave_ComputeChecksum(&temp_s3->playerName, &temp_s3->checksumBlock.checksum);
    temp_s3->checksumBlock.checksum ^= 0xFF;

    for (i = 0; i < 4; i++) {
        if ((GbPak_MbcWrite(arg0, &temp_s3->checksumBlock, 0x3520, sizeof(GbSaveChecksumBlock)) == 0) &&
            (GbPak_MbcRead(arg0, &D_800ACA78, 0x3520, sizeof(GbSaveChecksumBlock)) == 0) &&
            (GbSave_VerifyWriteback(&temp_s3->checksumBlock, sizeof(GbSaveChecksumBlock)) != 0)) {
            return 1;
        }
    }
    return 0;
}

s32 GbSave_ReadBoxBank(s32 arg0, s32 arg1) {
    UNUSED s32 pad[2];
    GbSaveBoxPair* sp1C = &D_800AC910[arg0].boxData->banks[arg1];

    return GbPak_MbcRead(arg0, sp1C, (arg1 == 0) ? 0x4000 : 0x6000, 0x1A60) == 0;
}

s32 GbSave_WriteBoxBank(s32 arg0, s32 arg1) {
    s32 var_v0;
    GbSaveBoxBank* temp_s2 = &D_800AC910[arg0].boxData->banks[arg1];
    s32 i;

    if (arg1 == 0) {
        var_v0 = 0x4000;
    } else {
        var_v0 = 0x6000;
    }

    var_v0 &= 0xFFFF;

    for (i = 0; i < 4; i++) {
        if ((GbPak_MbcWrite(arg0, temp_s2, var_v0, 0x1A60) == 0) &&
            (GbPak_MbcRead(arg0, &D_800ACA78, var_v0, 0x1A60) == 0) && (GbSave_VerifyWriteback(temp_s2, 0x1A60) != 0)) {
            return 1;
        }
    }
    return 0;
}

void GbSave_ReleasePortBuffers(UNUSED MainPoolBlock* arg0, u32 arg1) {
    GbSavePort* ptr;

    if ((arg1 >= 'DAT0') && (arg1 < 'DAT4')) {
        ptr = D_800AC910 + (arg1 - 'DAT0');

        ptr->mainData = NULL;
        ptr->flags &= ~0x9;
    } else if ((arg1 >= 'BOX0') && (arg1 < 'BOX4')) {
        ptr = D_800AC910 + (arg1 - 'BOX0');

        ptr->boxData = NULL;
        ptr->flags &= ~0x12;
    }
}

void GbSave_CheckLocationFlag(s32 arg0) {
    extern u8 D_80073640[12];

    s32 i;
    s32 temp_t1 = D_800AC910[arg0].mainData->currentLocationId;

    D_800AC910[arg0].flags &= ~0x4000;

    for (i = 0; i < 12; i++) {
        if (D_80073640[i] == temp_t1) {
            D_800AC910[arg0].flags |= 0x4000;
            break;
        }
    }
}

s32 GbSave_OpenMainData(s32 arg0) {
    u8 sp2F;
    s32 sp28 = 0;

    if (D_800AC910[arg0].flags & 1) {
        return 1;
    }

    D_800AC910[arg0].mainData = main_pool_alloc_with_func(0xFC0, 0, arg0 + 'DAT0', GbSave_ReleasePortBuffers);

    if (D_800AC910[arg0].mainData != NULL) {
        D_800AC910[arg0].flags |= 1;
        if (GbTower_ProbePak(arg0, &sp2F, D_800AC910[arg0].pakId) == 0) {
            Game_ShutdownAndLoadFragment(arg0, 2);
        }

        sp28 = GbSave_ReadMainData(arg0);
        if (GbTower_CheckPakRemoved(arg0, sp2F) == 0) {
            Game_ShutdownAndLoadFragment(arg0, 2);
        }

        if (sp28 == 0) {
            main_pool_try_free(D_800AC910[arg0].mainData);
        }
    }

    return sp28;
}

s32 GbSave_EnsureMainDataLoaded(s32 arg0) {
    s32 sp1C = 0;

    if (((1 << arg0) & D_800ACA70) && (D_800AC910[arg0].pakState == 0)) {
        sp1C = GbSave_OpenMainData(arg0);
        if (sp1C == 0) {
            Game_ShutdownAndLoadFragment(arg0, 2);
        }
    }

    return sp1C;
}

s32 GbSave_OpenBoxData(s32 arg0) {
    u8 sp37;
    s32 i;
    s32 sp2C;

    sp2C = 0;

    if (!(D_800AC910[arg0].flags & 1)) {
        return 0;
    }

    if (D_800AC910[arg0].flags & 2) {
        return 1;
    }

    D_800AC910[arg0].boxData = main_pool_alloc_with_func(sizeof(GbSaveBoxData), 0, arg0 + 'BOX0', GbSave_ReleasePortBuffers);

    if (D_800AC910[arg0].boxData != NULL) {
        D_800AC910[arg0].flags |= 2;
        if (GbSave_GetBoxCount(arg0) > 0) {
            if (GbTower_ProbePak(arg0, &sp37, D_800AC910[arg0].pakId) == 0) {
                Game_ShutdownAndLoadFragment(arg0, 2);
            }

            if (GbSave_ReadBoxBank(arg0, 0) != 0) {
                sp2C = GbSave_ReadBoxBank(arg0, 1);
            }

            if (GbTower_CheckPakRemoved(arg0, sp37) == 0) {
                Game_ShutdownAndLoadFragment(arg0, 2);
            }

            if (sp2C == 0) {
                main_pool_try_free(D_800AC910[arg0].boxData);
            }
        } else {
            HAL_Memset(D_800AC910[arg0].boxData, 0xFF, sizeof(GbSaveBoxData));

            for (i = 0; i < 6; i++) {
                D_800AC910[arg0].boxData->banks[0].pairs[i].primary.count = 0;
                D_800AC910[arg0].boxData->banks[1].pairs[i].primary.count = 0;
            }

            sp2C = 1;
        }
    }
    return sp2C;
}

s32 GbSave_LoadPort(s32 arg0) {
    u8 sp2F;
    s32 temp_s1 = 0;

    if ((1 << arg0) & D_800ACA70) {
        if (D_800AC910[arg0].pakState == 0) {
            if (GbTower_ProbePak(arg0, &sp2F, D_800AC910[arg0].pakId) == 0) {
                Game_ShutdownAndLoadFragment(arg0, 2);
            }

            temp_s1 = GbSave_EnsureMainDataLoaded(arg0);
            temp_s1 &= GbSave_OpenBoxData(arg0);

            if (GbTower_CheckPakRemoved(arg0, sp2F) == 0) {
                Game_ShutdownAndLoadFragment(arg0, 2);
            }

            if (temp_s1 != 0) {
                GbSave_RebuildSeenBitfield(arg0);
            }
        }
    }

    return temp_s1;
}

s32 GbSave_PollCartRemoval(s32 arg0) {
    extern u8 D_8007364C[];

    s32 sp24 = 0;

    switch (D_8007364C[arg0]) {
        case 0:
            GbTower_CheckPakRemoved(arg0, 0);
            D_8007364C[arg0] += 1;
            // fallthrough

        case 1:
            if (GbPak_IsCartOff(arg0) != 0) {
                D_8007364C[arg0] += 1;
            }
            break;

        case 2:
            if (GbPak_IsCartOn(arg0) != 0) {
                D_8007364C[arg0] += 1;
            }
            sp24 = 1;
            break;

        case 3:
            if (GbTower_ReinitPak(arg0) != 0) {
                sp24 = 3;
                if (GbCart_IdentifyGame(arg0) == D_800AC910[arg0].cartId) {
                    if (GbTower_VerifyPakStillInserted(arg0, D_800AC910[arg0].pakId) != 0) {
                        sp24 = 2;
                    }
                }
            }
            D_8007364C[arg0] = 0;
            break;
    }
    return sp24;
}

void GbSave_RefreshPortState(s32 arg0) {
    D_800AC910[arg0].flags &= ~0x7000;

    if (GbSave_OpenMainData(arg0) != 0) {
        GbSave_RefreshPlayerIdentity(arg0);
        GbSave_CheckLocationFlag(arg0);
        D_800AC910[arg0].partyCount = D_800AC910[arg0].mainData->party.count;

        if (D_800AC910[arg0].mainData->pokedexFlags & 0x20) {
            D_800AC910[arg0].flags |= 0x2000;
        }

        if (D_800AC910[arg0].mainData->progressFlags & 1) {
            D_800AC910[arg0].flags |= 0x1000;
        }

        D_800AC910[arg0].pakState = 0;
    } else {
        D_800AC910[arg0].pakState = 3;
    }
}

void GbSave_ForceReloadPort(s32 arg0) {
    if ((1 << arg0) & D_800ACA70) {
        D_800AC910[arg0].flags |= 0x20;
        GbSave_RefreshPortState(arg0);
    }
}

s32 GbSave_ScanAllPorts(void) {
    s32 i;
    s32 j;
    s32 temp_v0 = GbTower_ScanPaks();

    D_800ACA74 = temp_v0 & 0xFF;
    D_800ACA75 = (temp_v0 >> 0x10) & 0xFF;
    D_800ACA70 = 0;

    for (i = 0; i < 4; i++) {
        GbSavePort* var_s0 = &D_800AC910[i];
        u64* temp;

        var_s0->flags = 0x20;
        var_s0->mainData = NULL;
        var_s0->boxData = NULL;
        var_s0->pakState = 1;
        var_s0->cartId = 0;
        var_s0->partyCount = 0;

        temp = &var_s0->playerIdentity;
        *temp++ = 0x500000000000;
        *temp++ = 0;

        if (D_800ACA75 & (1 << i)) {
            var_s0->pakState = 2;
            var_s0->cartId = GbCart_IdentifyGame(i);
            if (var_s0->cartId) {
                D_800ACA70 |= (1 << i);
                if (GbPak_DetectRam(i, var_s0->pakId) == 0) {
                    Game_ShutdownAndLoadFragment(i, 2);
                }
                GbSave_RefreshPortState(i);
            }
            GbTower_CheckPakRemoved(i, 0);
        }
    }

    if (D_800ACA74 != 0) {
        GbTower_SetPollMask(D_800ACA70);
    }

    return D_800ACA70;
}

void GbSave_MarkBoxDataLoaded(s32 arg0) {
    u16 temp_v0 = GbData_ReadU16BE(&D_800AC910[arg0].mainData->boxDataStatus);

    if (!(temp_v0 & 0x8000)) {
        GbData_WriteU16BE(&D_800AC910[arg0].mainData->boxDataStatus, temp_v0 | 0x8000);
    }
}

s32 GbSave_WriteBoxBanks(s32 arg0) {
    u8 sp2F;
    s32 sp28 = 0;

    if (D_800AC910[arg0].flags & 2) {
        if (GbTower_ProbePak(arg0, &sp2F, D_800AC910[arg0].pakId) == 0) {
            Game_ShutdownAndLoadFragment(arg0, 2);
        }

        sp28 = GbSave_WriteBoxBank(arg0, 0);

        if (sp28 != 0) {
            sp28 = GbSave_WriteBoxBank(arg0, 1);
        }
        sp28 &= GbTower_CheckPakRemoved(arg0, sp2F);
    }
    return sp28;
}

s32 GbSave_CommitMainData(s32 arg0) {
    u8 sp2F;
    s32 sp28 = 0;

    if (D_800AC910[arg0].flags & 1) {
        if (GbTower_ProbePak(arg0, &sp2F, D_800AC910[arg0].pakId) == 0) {
            Game_ShutdownAndLoadFragment(arg0, 2);
        }

        sp28 = GbSave_WriteMainData(arg0);
        sp28 &= GbTower_CheckPakRemoved(arg0, sp2F);

        if (sp28 != 0) {
            GbSave_RefreshPlayerIdentity(arg0);
            D_800AC910[arg0].partyCount = D_800AC910[arg0].mainData->party.count;
        }
    }
    return sp28;
}

s32 GbSave_RebuildAndWriteSave(s32 arg0) {
    u8 sp2F;
    s32 sp28;

    if (GbTower_ProbePak(arg0, &sp2F, D_800AC910[arg0].pakId) == 0) {
        Game_ShutdownAndLoadFragment(arg0, 2);
    }

    sp28 = GbSave_InvalidateMainDataChecksum(arg0);

    if (sp28 != 0) {
        sp28 = GbSave_WriteBoxBanks(arg0);
        if (sp28 != 0) {
            sp28 = GbSave_CommitMainData(arg0);
        }
    }

    sp28 &= GbTower_CheckPakRemoved(arg0, sp2F);
    return sp28;
}

void GbSave_ValidateBeforeWrite(s32 arg0) {
    u8 sp27;

    if (GbTower_ProbePak(arg0, &sp27, D_800AC910[arg0].pakId) == 0) {
        Game_ShutdownAndLoadFragment(arg0, 2);
    }

    if (GbSave_CheckMainDataUnchanged(arg0) == 0) {
        Game_ShutdownAndLoadFragment(arg0, 1);
    }
}

void GbSave_RequirePakPresent(s32 arg0) {
    if (GbTower_CheckPakRemoved(arg0, 0) == 0) {
        Game_ShutdownAndLoadFragment(arg0, 2);
    }
}

GbSaveBoxEntry* GbSave_GetBoxEntry(s32 arg0, s32 arg1) {
    GbSaveBoxEntry* sp1C = NULL;
    s32 temp_v0 = GbSave_GetBoxCount(arg0);

    if (temp_v0 > 0) {
        if (arg1 == (temp_v0 - 1)) {
            sp1C = &D_800AC910[arg0].mainData[0].currentBox;
        } else if (arg1 < 6) {
            sp1C = &D_800AC910[arg0].boxData->banks[0].pairs[arg1].primary;
        } else {
            sp1C = &D_800AC910[arg0].boxData->banks[0].pairs[arg1].secondary;
        }
    }
    return sp1C;
}

void GbSave_LoadMainPokemon(BattleMon* arg0, s32 arg1, s32 arg2) {
    GbSaveParty* ptr2 = &D_800AC910[arg1].mainData->party;

    Pokemon_FromGbRecordExt(arg0, &ptr2->mons[arg2]);
    Pokemon_RecalcStats(arg0);

    arg0->sourceAndFlags = 0;
    arg0->sourceSlot = 0;
    arg0->faintOrder = 0;

    _bcopy(ptr2->nicknames[arg2], arg0->otNameEncoded, 0xB);
    Text_UntranscodeFixedName(arg0->nickname, ptr2->otNames[arg2]);
    Text_UntranscodeFixedName(arg0->otName, ptr2->nicknames[arg2]);
}

void GbSave_SaveMainPokemon(unk_func_88205880_00D0* arg0, s32 arg1, s32 arg2) {
    GbSaveParty* sp24 = &D_800AC910[arg1].mainData->party;

    Pokemon_ToGbRecordExt(arg0, &sp24->mons[arg2]);
    Text_TranscodeName(sp24->otNames[arg2], arg0->unk_000[0].nickname);
    _bcopy(arg0->unk_000[0].otNameEncoded, sp24->nicknames[arg2], 0xB);
    sp24->speciesIndices[arg2] = sp24->mons[arg2].base.speciesInternalIndex;
}

void GbSave_LoadBoxPokemon(BattleMon* arg0, s32 arg1, s32 arg2, s32 arg3) {
    GbSaveCurrentBox* sp2C = (GbSaveCurrentBox*)GbSave_GetBoxEntry(arg1, arg2);

    if (sp2C != NULL) {
        Pokemon_FromGbRecord(arg0, &sp2C->mons[arg3]);
        Pokemon_RecalcStats(arg0);

        arg0->sourceAndFlags = 0;
        arg0->sourceSlot = 0;
        arg0->faintOrder = 0;

        _bcopy(sp2C->otNames[arg3], arg0->otNameEncoded, 0xB);
        Text_UntranscodeFixedName(arg0->nickname, sp2C->nicknames[arg3]);
        Text_UntranscodeFixedName(arg0->otName, sp2C->otNames[arg3]);
    }
}

void GbSave_SaveBoxPokemon(unk_func_88205880_00D0* arg0, s32 arg1, s32 arg2, s32 arg3) {
    GbSaveCurrentBox* sp2C = GbSave_GetBoxEntry(arg1, arg2);

    if (sp2C != NULL) {
        arg0->unk_000[0].boxLevel = arg0->unk_000[0].level;
        Pokemon_ToGbRecord(arg0->unk_000, &sp2C->mons[arg3]);
        Text_TranscodeNameWrapper(sp2C->nicknames[arg3], arg0->unk_000[0].nickname);
        _bcopy(arg0->unk_000[0].otNameEncoded, sp2C->otNames[arg3], 0xB);
        sp2C->header.speciesIndices[arg3] = sp2C->mons[arg3].speciesInternalIndex;
    }
}

s32 GbSave_GetBoxEntryCount(s32 arg0, s32 arg1) {
    s32 sp1C = 0;
    GbSaveBoxEntry* temp_v0 = GbSave_GetBoxEntry(arg0, arg1);

    if (temp_v0 != NULL) {
        sp1C = temp_v0->count;
    }
    return sp1C;
}

void GbSave_SetBoxEntryCount(s32 arg0, s32 arg1, s32 arg2) {
    GbSaveBoxEntry* temp_v0 = GbSave_GetBoxEntry(arg0, arg1);

    if (temp_v0 != NULL) {
        temp_v0->count = arg2;
        temp_v0->speciesIndices[arg2] = 0xFF;
    }
}

s32 Deck_GetCapacity(s32 arg0) {
    s32 ret = 0;

    switch (arg0) {
        case 32:
            ret = 6;
            break;

        case 33:
            ret = 0x1E;
            break;

        case 34:
            ret = 0x32;
            break;

        case 35:
            ret = 0x14;
            break;
    }

    return ret;
}

s32 Deck_GetEntrySize(s32 arg0) {
    s32 ret = 0;

    switch (arg0) {
        case 32:
            ret = 0x54;
            break;
        case 33:
            ret = 0x54;
            break;
        case 34:
            ret = 2;
            break;
        case 35:
            ret = 2;
            break;
    }
    return ret;
}

s32 Deck_GetCount(s32 arg0, s32 arg1, s32 arg2) {
    s32 var_v1 = 0;
    GbSaveMainData* ptr = D_800AC910[arg1].mainData;

    switch (arg0) {
        case 32:
            var_v1 = ptr->party.count;
            break;

        case 33:
            var_v1 = GbSave_GetBoxEntryCount(arg1, arg2);
            break;

        case 34:
            var_v1 = ptr->unk_266;
            break;

        case 35:
            var_v1 = ptr->unk_049;
            break;
    }
    return var_v1;
}

void Deck_DeleteEntry(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    GbSaveMainData* sp1C = D_800AC910[arg1].mainData;

    if (arg3 < Deck_GetCount(arg0, arg1, arg2)) {
        switch (arg0) {
            case 32:
                sp1C->party.count = arg3;
                sp1C->party.speciesIndices[arg3] = 0xFF;
                break;

            case 33:
                GbSave_SetBoxEntryCount(arg1, arg2, arg3);
                break;

            case 34:
                sp1C->unk_266 = arg3;
                sp1C->unk_267[arg3].dexId = 0xFF;
                break;

            case 35:
                sp1C->unk_049 = arg3;
                sp1C->unk_04A[arg3].dexId = 0xFF;
                break;
        }
    }
}

void Deck_AppendEntry(s32 arg0, s32 arg1, s32 arg2) {
    GbSaveMainData* sp1C = D_800AC910[arg1].mainData;
    s32 sp18 = Deck_GetCount(arg0, arg1, arg2);

    if (sp18 < Deck_GetCapacity(arg0)) {
        sp18++;

        switch (arg0) {
            case 32:
                sp1C->party.count = sp18;
                sp1C->party.speciesIndices[sp18] = 0xFF;
                break;

            case 33:
                GbSave_SetBoxEntryCount(arg1, arg2, sp18);
                break;

            case 34:
                sp1C->unk_266 = sp18;
                sp1C->unk_267[sp18].dexId = 0xFF;
                break;

            case 35:
                sp1C->unk_049 = sp18;
                sp1C->unk_04A[sp18].dexId = 0xFF;
                break;
        }
    }
}

s32 Deck_ReadEntry(s32 arg0, s32 arg1, s32 arg2, s32 arg3, BattleMon* arg4) {
    s32 sp1C = 0;

    if (arg3 < Deck_GetCount(arg0, arg1, arg2)) {
        GbSaveMainData* temp_v0 = D_800AC910[arg1].mainData;

        switch (arg0) {
            case 32:
                GbSave_LoadMainPokemon(arg4, arg1, arg3);
                break;

            case 33:
                GbSave_LoadBoxPokemon(arg4, arg1, arg2, arg3);
                break;

            case 35:
                arg4->species = temp_v0->unk_04A[arg3];
                break;

            case 34:
                arg4->species = temp_v0->unk_267[arg3];
                break;
        }
        sp1C = 1;
    }
    return sp1C;
}

s32 Deck_WriteEntry(s32 arg0, s32 arg1, s32 arg2, s32 arg3, unk_func_88205880_00D0* arg4) {
    s32 sp2C = 0;
    s32 sp28 = Deck_GetCapacity(arg0);
    s32 sp24 = Deck_GetCount(arg0, arg1, arg2);

    if ((arg3 < sp24) || ((arg3 == sp24) && (arg3 < sp28))) {
        GbSaveMainData* ptr = D_800AC910[arg1].mainData;

        switch (arg0) {
            case 32:
                GbSave_SaveMainPokemon(arg4, arg1, arg3);
                break;

            case 33:
                GbSave_SaveBoxPokemon(arg4, arg1, arg2, arg3);
                break;

            case 35:
                ptr->unk_04A[arg3] = arg4->unk_000[0].species;
                break;

            case 34:
                ptr->unk_267[arg3] = arg4->unk_000[0].species;
                break;
        }

        if (arg3 == sp24) {
            Deck_AppendEntry(arg0, arg1, arg2);
        }

        sp2C = 1;
    }
    return sp2C;
}

s32 GbSave_GetPresentPortMask(void) {
    return D_800ACA70;
}

s32 GbSave_GetSaveState(s32 arg0) {
    return D_800AC910[arg0].pakState;
}

s32 GbSave_GetPortAvailability(s32 arg0) {
    s32 temp_v0 = 1 << arg0;

    if (!(D_800ACA74 & temp_v0)) {
        return 1;
    }

    if (!(D_800ACA75 & temp_v0)) {
        return 2;
    }

    return 0;
}

s32 GbSave_GetPortGame(s32 arg0) {
    return D_800AC910[arg0].cartId;
}

s32 GbSave_GetPartyCount(s32 arg0) {
    return D_800AC910[arg0].partyCount;
}

s32 GbSave_RefreshPlayerIdentity(s32 arg0) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return 0;
    }

    D_800AC910[arg0].playerIdentity.trainerId = GbData_ReadU16BE(&D_800AC910[arg0].mainData->trainerId);
    _bcopy(&D_800AC910[arg0].mainData->playerName, &D_800AC910[arg0].playerIdentity.playerName, 0xB);
    bzero(D_800AC910[arg0].playerIdentity.unk_0D, 3);
    return 1;
}

void GbSave_CopyPlayerIdentity(s32 arg0, GbSavePlayerIdentity* arg1) {
    u64* ptr = &D_800AC910[arg0].playerIdentity;
    u64* ptr2 = arg1;

    *ptr2++ = *ptr++;
    *ptr2++ = *ptr++;
}

s32 GbSave_GetTrainerId(s32 arg0) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return -1;
    }
    return GbData_ReadU16BE(&D_800AC910[arg0].mainData->trainerId);
}

void GbSave_SetTrainerId(s32 arg0, u16 arg1) {
    if (D_800AC910[arg0].flags & 1) {
        GbData_WriteU16BE(&D_800AC910[arg0].mainData->trainerId, arg1);
    }
}

s32 GbSave_GetBoxCount(s32 arg0) {
    s32 temp_a0;
    s32 phi_v1 = -1;

    if (D_800AC910[arg0].flags & 1) {
        temp_a0 = GbData_ReadU16BE(&D_800AC910[arg0].mainData->boxDataStatus) >> 8;
        if (temp_a0 & 0x80) {
            phi_v1 = (temp_a0 & 0x7F) + 1;
        } else {
            phi_v1 = 0;
        }
    }
    return phi_v1;
}

char* GbSave_GetPlayerName(s32 arg0, char* arg1) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return NULL;
    }
    return Text_UntranscodeFixedName(arg1, &D_800AC910[arg0].mainData->playerName);
}

char* GbSave_SetPlayerName(s32 arg0, char* arg1) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return NULL;
    }
    return Text_TranscodeName(&D_800AC910[arg0].mainData->playerName, arg1);
}

char* GbSave_GetSecondaryName(s32 arg0, char* arg1) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return NULL;
    }
    return Text_UntranscodeFixedName(arg1, &D_800AC910[arg0].mainData->secondaryName);
}

char* GbSave_SetSecondaryName(s32 arg0, char* arg1) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return NULL;
    }
    return Text_TranscodeName(&D_800AC910[arg0].mainData->secondaryName, arg1);
}

s32 GbSave_GetStarterChoice(s32 arg0) {
    if (!(D_800AC910[arg0].flags & 1)) {
        return -1;
    }
    return D_800AC910[arg0].mainData->starterChoice;
}

s32 GbSave_SavedAtPokemonCenter(s32 arg0) {
    return (D_800AC910[arg0].flags & 0x4000) != 0;
}

s32 GbSave_HasPokedex(s32 arg0) {
    return (D_800AC910[arg0].flags & 0x2000) != 0;
}

s32 GbSave_HasProgressFlag1000(s32 arg0) {
    return (D_800AC910[arg0].flags & 0x1000) != 0;
}

s32 GbSave_CopyPortSnapshot(s32 arg0, GbSavePortSnapshot* arg1) {
    s32 temp_v1 = (D_800AC910[arg0].flags & 1) != 0;

    if (temp_v1 != 0) {
        arg1->cartId = D_800AC910[arg0].cartId;
        arg1->mainDataChecksum = D_800AC910[arg0].mainDataChecksum;
        arg1->trainerId = D_800AC910[arg0].playerIdentity.trainerId;

        _bcopy(D_800AC910[arg0].playerIdentity.playerName, arg1->playerName, sizeof(arg1->playerName));
        _bcopy(&D_800AC910[arg0].mainData->party, (u8*)arg1 + 0xF, sizeof(GbSaveParty));
    }
    return temp_v1;
}

void GbSave_MarkSeenBits(char* arg0, s32 arg1, u8* arg2) {
    s32 temp_v0;
    s32 h;

    while (arg1-- > 0) {
        temp_v0 = Species_FromInternalIndex(*arg2++);
        if (0x98 != temp_v0) {
            temp_v0++;
            temp_v0--;
            temp_v0--;
            h = temp_v0 / 8;
            arg0[h] |= (1 << (temp_v0 % 8));
        }
    }
}

void GbSave_RebuildSeenBitfield(s32 arg0) {
    GbSavePort* ptr = &D_800AC910[arg0];
    GbSaveMainData* temp_s0 = ptr->mainData;
    u8* ptr6 = ptr->stadiumEncounteredBitmap;
    GbSaveBoxBank* ptr541 = &ptr->boxData->banks[0];
    GbSaveBoxBank* ptr542 = &ptr->boxData->banks[1];
    s32 i;

    for (i = 0; i < 19; i++) {
        ptr6[i] = 0;
    }

    GbSave_MarkSeenBits(ptr6, temp_s0->party.count, temp_s0->party.speciesIndices);
    GbSave_MarkSeenBits(ptr6, temp_s0->currentBox.header.count, temp_s0->currentBox.header.speciesIndices);

    for (i = 0; i < 6; i++) {
        GbSave_MarkSeenBits(ptr6, ptr541->pairs[i].primary.count, ptr541->pairs[i].primary.speciesIndices);
        GbSave_MarkSeenBits(ptr6, ptr542->pairs[i].primary.count, ptr542->pairs[i].primary.speciesIndices);
    }
}

s32 GbSave_GetSpeciesProgressLevel(s32 arg0, s32 arg1) {
    s32 temp_t1;
    s32 temp_t2;
    u8* ptr23;
    u8* ptr36;

    if ((arg1 > 0) && (arg1 < 0x98)) {
        ptr23 = D_800AC910[arg0].mainData->ownedBitmap;
        ptr36 = D_800AC910[arg0].mainData->seenBitmap;

        temp_t1 = (arg1 - 1) / 8;
        temp_t2 = (arg1 - 1) % 8;

        return ((ptr23[temp_t1] >> temp_t2) & 1) + ((ptr36[temp_t1] >> temp_t2) & 1) +
               ((D_800AC910[arg0].stadiumEncounteredBitmap[temp_t1] >> temp_t2) & 1);
    }
    return 0;
}

void GbSave_SetSeenOwnedBits(s32 arg0, s32 arg1, s32 arg2) {
    u8* temp_a3;
    u8* temp_v0;
    s32 var_a2;
    s32 var_a3;
    u8* temp_v1;

    if ((arg1 > 0) && (arg1 < 0x98)) {
        temp_v0 = D_800AC910[arg0].mainData->ownedBitmap;
        temp_a3 = D_800AC910[arg0].mainData->seenBitmap;
        temp_v1 = &D_800AC910[arg0].stadiumEncounteredBitmap;

        var_a2 = (arg1 - 1) / 8;
        var_a3 = (arg1 - 1) % 8;

        if (arg2 > 0) {
            temp_a3[var_a2] |= (1 << var_a3);
        } else {
            temp_a3[var_a2] &= ~(1 << var_a3);
        }

        if (arg2 >= 2) {
            temp_v0[var_a2] |= (1 << var_a3);
        } else {
            temp_v0[var_a2] &= ~(1 << var_a3);
        }

        if (arg2 >= 3) {
            temp_v1[var_a2] |= (1 << var_a3);
        } else {
            temp_v1[var_a2] &= ~(1 << var_a3);
        }
    }
}

s32 GbSave_CountSpeciesProgressField036(s32 arg0) {
    s32 i;
    s32 temp_v1 = 0;

    for (i = 0; i < 0x97; i++) {
        temp_v1 += (D_800AC910[arg0].mainData->seenBitmap[i / 8] >> (i % 8)) & 1;
    }

    return temp_v1;
}

s32 GbSave_CountSpeciesProgressField023(s32 arg0) {
    s32 i;
    s32 temp_v1 = 0;

    for (i = 0; i < 0x97; i++) {
        temp_v1 += (D_800AC910[arg0].mainData->ownedBitmap[i / 8] >> (i % 8)) & 1;
    }

    return temp_v1;
}

s32 GbSave_CountSpeciesProgressField06(s32 arg0) {
    s32 i;
    s32 temp_v1 = 0;

    for (i = 0; i < 0x97; i++) {
        temp_v1 += (D_800AC910[arg0].stadiumEncounteredBitmap[i / 8] >> (i % 8)) & 1;
    }

    return temp_v1;
}
