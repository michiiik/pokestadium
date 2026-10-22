#include "minigame_select.h"
#include "src/fragments/widget_toolkit/widget_toolkit.h"
#include "save_data.h"

void KidsClub_LoadDifficultySetting(void) {
    gComHyperUnlocked = Save_GetOptionsField1E() & 0xFF;
}

void KidsClub_SetSelectedGameId(s16 arg0) {
    gCurrentMenuIndex = arg0;
}

void KidsClub_ResetTournamentState(s16 arg0) {
    s32 i;
    s32 j;

    for (i = 0; i < 4; i++) {
        gMenuOptionFlags[i] = 0;
    }

    switch (arg0) {
        case 0:
            gCurrentMenuIndex = -1;
            gMenuCursorX = 0;
            gMenuCursorY = 0;

            for (i = 0; i < 4; i++) {
                gMiniHudColorDefaults[i] = 1;
            }

            gMinigameComDifficulty = 1;
            gWinsToChampion = 3;

            for (i = 0; i < 4; i++) {
                gPlayerWinCounts[i] = 0;
            }

            gMenuSelectedOption = 0;
            // clang-format off
            for (i = 0; i < 9; i++) { gMenuOptionUsedFlags[i] = 0; }
            // clang-format on
            break;

        case 1:
        case 4:
            break;
    }
}

s16 KidsClub_PickUnplayedGame(void) {
    u32 var_a2;
    s32 i;
    s16 sp24[9];

    var_a2 = 0;

    for (i = 0; i < 9; i++) {
        if (gMenuOptionUsedFlags[i] == 0) {
            sp24[var_a2++] = i;
        }
    }

    if (var_a2 == 0) {
        var_a2 = 9;

        for (i = 0; i < 9; i++) {
            gMenuOptionUsedFlags[i] = 0;
            sp24[i] = i;
        }
    }

    return sp24[Rand_Range(var_a2)];
}
