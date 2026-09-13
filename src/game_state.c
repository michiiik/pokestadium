#include "game_state.h"
#include "include/functions.h"
#include "src/input.h"
#include "src/save_data.h"
#include "src/audio_commands.h"
#include "src/gallery.h"
#include "src/gb_data.h"
#include "src/jpeg_decoder.h"
#include "src/gfx_buffer.h"
#include "src/DDC0.h"
#include "src/session.h"
#include "src/save_data.h"
#include "src/minigame_dispatch.h"
#include "src/gb_save.h"
#include "src/gb_tower_audio.h"
#include "src/leo.h"
#include "src/display_object_textures.h"
#include "src/pokemon_stats.h"
#include "src/controller.h"
#include "src/dp_intro.h"
#include "src/gb_tower.h"
#include "src/memory.h"
#include "src/rsp.h"
#include "src/stage_loader.h"
#include "lib/ultralib/include/PR/os_internal_reg.h"

static s32 pad_D_80075660 = 0;
static s32 pad_D_80075664 = 0;
s32 gCurrentGameState = STATE_N64_LOGO_INTRO;
static s32 gLastGameState = STATE_N64_LOGO_INTRO;
static s32 pad_D_80075670 = 0;
static s16 D_80075674 = 0;
static s16 D_80075678 = 0;
static s16 D_8007567C = 0;

unk_D_800AE520 D_800AE520;
s32 pad_D_800AE534[2];
SessionContext D_800AE540;
BinArchive* D_800AF738;

s32 Game_DoCopyProtection(s32 state) {
    u32 intr = osSetIntMask(1);

    // if the 16-bit value at ROM offset 0xE38 is not 0x828A, make the state an invalid one.
    if (!(HW_REG(PI_STATUS_REG, u32) & 3) && ((*(u32*)0xB0000E38 & 0xFFFF) != 0x828A)) {
        state = -0x10;
    }

    osSetIntMask(intr);

    return state;
}

s32 Fragment_LoadAndCall(s32 arg0, u8* romStart, u8* romEnd, u32 arg3, u32 arg4) {
    s32 result;
    FragmentEntry func;

    func = Fragment_Load(arg0, romStart, romEnd);

    result = func(arg3, arg4);

    main_pool_try_free(func);

    return result;
}

void Fragment_QueueCall(s32 arg0, u8* arg1, u8* arg2, s32 arg3, s32 arg4) {
    D_800AE520.pendingReload = 1;
    D_800AE520.fragment_id = arg0;
    D_800AE520.rom_start = arg1;
    D_800AE520.rom_end = arg2;
    D_800AE520.arg0 = arg3;
    D_800AE520.arg1 = arg4;
}

TeamRoster* Trainer_GetSlots(void) {
    return D_800AE540.unk_0004;
}

s32 GbSave_GetActivePort(void) {
    return D_80075674;
}

void GbSave_SetActivePort(s32 arg0) {
    if ((arg0 >= 0) && (arg0 < 4)) {
        D_80075674 = arg0;
        D_80075678 = 1;
    }
}

void Controller_PollInputs(void) {
    Cont_StartReadInputs();
    Cont_ReadInputs();
    Input_ResetRepeatState();
}

s32 Stage_RunDemoScene(s16 arg0) {
    s32 sp24;

    main_pool_push_state('Demo');

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD(fragment2);
    FRAGMENT_LOAD(fragment3);

    D_800AE540.sessionMode = arg0;

    sp24 = FRAGMENT_LOAD_AND_CALL(fragment17, arg0, 0);

    main_pool_pop_state('Demo');

    return sp24;
}

void State_RunCredits(void) {
    main_pool_push_state('demo');

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD(fragment2);
    FRAGMENT_LOAD(fragment3);

    if (D_800AE540.roundSelector != 0) {
        FRAGMENT_LOAD_AND_CALL(fragment16, 0, 0);
    } else {
        FRAGMENT_LOAD_AND_CALL(fragment16, 0, 1);
    }

    main_pool_pop_state('demo');
}

void State_N64LogoIntro(void) {
    FRAGMENT_LOAD_AND_CALL(fragment35, 0, 0);

    main_pool_push_state('TITL');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    StageContext_Activate(StageContext_Allocate(0, 1, 3, 1, 2, 1));

    if (Stage_RunDemoScene(0x12) == 2) {
        StageFade_SetMode(1);
        StageLoader_RunFrames(2);
    }

    gCurrentGameState = STATE_TITLE_SCREEN;
}

void State_TitleScreen(void) {
    gCurrentGameState = FRAGMENT_LOAD_AND_CALL(fragment36, 0, &D_8007567C);

    if (gCurrentGameState == STATE_TITLE_SCREEN) {
        main_pool_push_state('DEMO');

        if (D_800AE540.sessionMode == 0x10) {
            FRAGMENT_LOAD(fragment34);
            FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);
        } else {
            ASSET_LOAD(D_3000000, kids_club_game_ui, 0); // borrows the Stadium logo from it
            FRAGMENT_LOAD(fragment2);
            FRAGMENT_LOAD(fragment34);

            switch (D_800AE540.opponentNumber) {
                case 3:
                    FRAGMENT_LOAD(fragment4);
                    FRAGMENT_LOAD_AND_CALL(fragment8, 1, 0);
                    break;

                case 9:
                    FRAGMENT_LOAD(fragment3);
                    FRAGMENT_LOAD_AND_CALL(fragment14, 1, 0);
                    break;

                case 13:
                    FRAGMENT_LOAD(fragment3);
                    FRAGMENT_LOAD_AND_CALL(fragment18, 1, 0);
                    break;
            }
        }

        main_pool_pop_state('DEMO');
        return;
    }

    D_800AE540.progressIndex = 0;
    D_800AE540.battleFlowFlags = 0;
    D_80075674 = -1;
}

void State_N64ddBootUnused(void) {
    Audio_WaitForIdle(1);
    Display_WaitForFrames(2);
    LeoBootGame(D_800AA680.payloadAddr);
}

void State_StubbedDebug(void) {
    s32 temp_v0;
    unk_func_80007444* temp_s0;

    main_pool_push_state('DBUG');

    Gfx_InitDisplayListBuffers(0x10000, 0);
    temp_s0 = StageContext_Allocate(1, 1, 2, 0, 2, 1);

    FRAGMENT_LOAD(fragment34);
    StageContext_Activate(temp_s0);
    FRAGMENT_LOAD_AND_CALL2(fragment67, 1, 0);

    while ((gCurrentGameState == STATE_STUBBED_DEBUG) && (D_800AE520.pendingReload != 0)) {
        D_800AE520.pendingReload = 0;
        gCurrentGameState = Fragment_LoadAndCall(D_800AE520.fragment_id, D_800AE520.rom_start, D_800AE520.rom_end,
                                          D_800AE520.arg0, D_800AE520.arg1);
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('DBUG');
}

void State_AreaSelect(void) {
    gCurrentGameState = FRAGMENT_LOAD_AND_CALL(fragment37, 0, 0);
    D_80075674 = -1;
}

void State_EventBattle(void) {
    gCurrentGameState = FRAGMENT_LOAD_AND_CALL(fragment38, 0, 0);
}

void State_Options(void) {
    gCurrentGameState = FRAGMENT_LOAD_AND_CALL(fragment56, 0, 0);
}

void State_MenuSelect(void) {
    s32 from_title_screen = gLastGameState == STATE_TITLE_SCREEN;

    gCurrentGameState = FRAGMENT_LOAD_AND_CALL(fragment57, 0, from_title_screen);
}

s16 RuleWizard_StadiumMenu(s16 arg0) {
    main_pool_push_state('STAD');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    StageContext_Activate(StageContext_Allocate(1, 1, 2, 0, 2, 1));

    while ((arg0 > 0) && (arg0 < 5)) {
        switch (arg0) {
            case 1:
                if (FRAGMENT_LOAD_AND_CALL(fragment59, 0, 0) != 0) {
                    arg0 = 2;
                } else {
                    arg0 = 0;
                }
                break;

            case 2:
                if (FRAGMENT_LOAD_AND_CALL(fragment60, 0, 0) != 0) {
                    arg0 = 3;
                } else {
                    arg0 = 1;
                }
                break;

            case 3:
                if (FRAGMENT_LOAD_AND_CALL(fragment54, 0, 0) != 0) {
                    arg0 = 4;
                } else {
                    arg0 = 1;
                }
                break;

            case 4:
                if (FRAGMENT_LOAD_AND_CALL(fragment61, 0, 0) != 0) {
                    arg0 = 5;
                } else {
                    arg0 = 3;
                }
                break;
        }
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('STAD');

    return arg0;
}

void State_StadiumMenu(void) {
    s16 sp4E = 1;

    while ((sp4E > 0) && (sp4E < 5)) {
        if (D_800AE540.sessionFlowFlags & 1) {
            sp4E = 5;
        } else {
            sp4E = RuleWizard_StadiumMenu(sp4E);
        }

        if (sp4E == 5) {
            D_800AE540.battleFlowFlags = 0;
            FRAGMENT_LOAD_AND_CALL(fragment63, 1, 0);

            while (!(D_800AE540.battleFlowFlags & 1)) {
                FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

                main_pool_push_state('STAD');

                FRAGMENT_LOAD(fragment34);
                FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

                main_pool_pop_state('STAD');

                FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);
            }

            if (D_800AE540.battleFlowFlags & 0x20) {
                sp4E = 4;
            }

            if (D_800AE540.battleFlowFlags & 0x800) {
                sp4E = 4;
            }

            if (D_800AE540.battleFlowFlags & 2) {
                Stage_RunDemoScene(0x11);
            }

            if (D_800AE540.battleFlowFlags & 0x1000) {
                FRAGMENT_LOAD_AND_CALL(fragment66, 0, 0);
            }

            if (D_800AE540.battleFlowFlags & 8) {
                Stage_RunDemoScene(0x16);
            }

            if (D_800AE540.battleFlowFlags & 0x10) {
                Stage_RunDemoScene(0x17);
            }
        }
    }

    if (D_800AE540.battleFlowFlags & 0x40) {
        gCurrentGameState = STATE_TITLE_SCREEN;
    } else {
        gCurrentGameState = STATE_AREA_SELECT;
    }
}

s32 RuleWizard_FreeBattle(s16 arg0) {
    main_pool_push_state('FREE');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    StageContext_Activate(StageContext_Allocate(1, 1, 2, 0, 2, 1));

    while ((arg0 > 0) && (arg0 < 4)) {
        switch (arg0) {
            case 1:
                if (FRAGMENT_LOAD_AND_CALL(fragment58, 0, 0) != 0) {
                    arg0 = 2;
                } else {
                    arg0 = 0;
                }
                break;

            case 2:
                if (FRAGMENT_LOAD_AND_CALL(fragment54, 0, 0) != 0) {
                    arg0 = 3;
                } else {
                    arg0 = 1;
                }
                break;

            case 3:
                if (FRAGMENT_LOAD_AND_CALL(fragment61, 0, 1) != 0) {
                    arg0 = 4;
                } else {
                    arg0 = 2;
                }
                break;
        }
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();
    main_pool_pop_state('FREE');

    return arg0;
}

void State_FreeBattle(void) {
    s16 sp4E = 1;

    while ((sp4E > 0) && (sp4E < 4)) {
        sp4E = RuleWizard_FreeBattle(sp4E);

        switch (sp4E) {
            case 4:
                D_800AE540.battleFlowFlags = 0;
                FRAGMENT_LOAD_AND_CALL(fragment63, 0, 0);

                while (!(D_800AE540.battleFlowFlags & 1)) {
                    FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

                    main_pool_push_state('FREE');

                    FRAGMENT_LOAD(fragment34);
                    FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

                    main_pool_pop_state('FREE');

                    FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);
                }

                if (D_800AE540.battleFlowFlags & 0x20) {
                    sp4E = 3;
                }

                if (D_800AE540.battleFlowFlags & 0x80) {
                    sp4E = 1;
                }
                break;
        }
    }

    gCurrentGameState = STATE_AREA_SELECT;
}

s16 RuleWizard_VsMewtwo(s16 arg0) {
    main_pool_push_state('STAD');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    StageContext_Activate(StageContext_Allocate(1, 1, 2, 0, 2, 1));

    while ((arg0 > 0) && (arg0 < 3)) {
        switch (arg0) {
            case 1:
                if (FRAGMENT_LOAD_AND_CALL(fragment54, 0, 0) != 0) {
                    arg0 = 2;
                } else {
                    arg0 = 0;
                }
                break;

            case 2:
                if (FRAGMENT_LOAD_AND_CALL(fragment61, 0, 0) != 0) {
                    arg0 = 3;
                } else {
                    arg0 = 1;
                }
                break;
        }
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('STAD');
    return arg0;
}

void State_VsMewtwo(void) {
    s16 sp4E = 1;

    Stage_RunDemoScene(0x18);
    Session_SetMode(8, 8, 0);
    Team_AddTrainer(0, Trainer_Create(0, 0, "1P", Text_GetPlayerLabel(1)));
    Team_AddTrainer(1, Trainer_Create(-1, 0, "COM", Text_GetPlayerLabel(3)));

    while ((sp4E > 0) && (sp4E < 3)) {
        sp4E = RuleWizard_VsMewtwo(sp4E);

        switch (sp4E) {
            case 3:
                D_800AE540.battleFlowFlags = 0;
                FRAGMENT_LOAD_AND_CALL(fragment63, 0, 0);

                while (!(D_800AE540.battleFlowFlags & 1)) {
                    FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

                    main_pool_push_state('STAD');

                    FRAGMENT_LOAD(fragment34);
                    FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

                    main_pool_pop_state('STAD');

                    FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);
                }

                if (D_800AE540.battleFlowFlags & 0x20) {
                    sp4E = 2;
                }

                if (D_800AE540.battleFlowFlags & 4) {
                    State_RunCredits();
                    gCurrentGameState = STATE_N64_LOGO_INTRO;
                    return;
                }
                break;
        }
    }

    gCurrentGameState = STATE_AREA_SELECT;
}

void State_KidsClub(void) {
    MiniGame_Dispatch();
    gCurrentGameState = STATE_AREA_SELECT;
}

void State_Gallery(void) {
    Gallery_Dispatch();
    gCurrentGameState = STATE_MENU_SELECT;
}

void State_VictoryPalace(void) {
    if (FRAGMENT_LOAD_AND_CALL(fragment50, 0, 0) == 1) {
        Stage_RunDemoScene(0x1A);
    }
    gCurrentGameState = STATE_AREA_SELECT;
}

void State_GbTower(void) {
    s32 sp34;
    GbTowerLaunchData sp2C;
    GameOptions sp24;

    main_pool_push_state('EMU0');

    if ((FRAGMENT_LOAD_AND_CALL(fragment33, 0, 0) == 0) && (D_80075674 >= 0) && (D_80075674 < 4)) {
        Audio_StopMusic(4);
        Audio_WaitForIdle(1);
        Display_WaitForFrames(2);
        Audio_Disable();

        sp2C.partyIconFrames = main_pool_alloc(sizeof(*sp2C.partyIconFrames), 0);

        FRAGMENT_LOAD_AND_CALL(fragment53, D_80075674, &sp2C);
        GbTower_Start(&sp2C);

        sp34 = GbTower_RecvExitMesg();

        GbTower_Stop(&sp2C);
        Display_WaitForFrames(2);
        Audio_Enable();

        if (sp34 != 0) {
            Game_ShutdownAndLoadFragment(D_80075674, sp34);
        }

        GbSave_ForceReloadPort(D_80075674);
        Save_EnsureBankLoaded(2);
        Save_GetOptions(&sp24);

        sp24.buttonBindingsPacked = sp2C.buttonBindingsPacked;

        Save_SetOptions(&sp24);
        Save_CommitTypedRecord(0x14, 0);
        Save_FlushBank(2);
    }

    main_pool_pop_state('EMU0');

    gCurrentGameState = STATE_AREA_SELECT;
}

s16 RuleWizard_GymLeaderCastle(s16 arg0, s16 arg1) {
    main_pool_push_state('STAD');

    Gfx_InitDisplayListBuffers(0x18000, 0);
    StageContext_Activate(StageContext_Allocate(1, 1, 2, 0, 2, 1));

    while ((arg0 > 0) && (arg0 < 4)) {
        switch (arg0) {
            case 1:
                if (FRAGMENT_LOAD_AND_CALL(fragment54, 0, 0) != 0) {
                    arg0 = 2;
                } else {
                    arg0 = 0;
                }
                break;

            case 2:
                if (FRAGMENT_LOAD_AND_CALL(fragment65, arg1, 0) != 0) {
                    arg0 = 3;
                } else {
                    arg0 = 1;
                }
                break;

            case 3:
                if (FRAGMENT_LOAD_AND_CALL(fragment61, 0, 0) != 0) {
                    arg0 = 4;
                } else {
                    arg0 = 2;
                }
                break;
        }

        arg1 = 0;
    }

    StageContext_Deactivate();
    Gfx_FreeDisplayListBuffers();

    main_pool_pop_state('STAD');

    return arg0;
}

void State_GymLeaderCastle(void) {
    s16 sp4E = 1;
    s16 sp4C = 0;

    if (!(D_800AE540.sessionFlowFlags & 1)) {
        Session_SetMode(7, 7, 0);
        Team_AddTrainer(0, Trainer_Create(0, 0, "1P", Text_GetPlayerLabel(1)));
        Team_AddTrainer(1, Trainer_Create(-1, 0, "COM", Text_GetPlayerLabel(3)));
    }

    while ((sp4E > 0) && (sp4E < 4)) {
        if (D_800AE540.sessionFlowFlags & 1) {
            sp4E = 4;
        } else {
            sp4E = RuleWizard_GymLeaderCastle(sp4E, sp4C);
        }

        if (sp4E == 4) {
            sp4C = 0;
            D_800AE540.battleFlowFlags = 0;

            FRAGMENT_LOAD_AND_CALL(fragment63, 1, 0);

            while (!(D_800AE540.battleFlowFlags & 1)) {
                FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

                main_pool_push_state('STAD');

                FRAGMENT_LOAD(fragment34);
                FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

                main_pool_pop_state('STAD');

                FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);
            }

            if (D_800AE540.battleFlowFlags & 0x200) {
                FRAGMENT_LOAD_AND_CALL(fragment40, 0, 0);
                Stage_RunDemoScene(0x11);
                Stage_RunDemoScene(0x15);
            }

            if (D_800AE540.battleFlowFlags & 0x20) {
                sp4E = 3;
            }

            if (D_800AE540.battleFlowFlags & 0x800) {
                sp4E = 3;
            }

            if (D_800AE540.battleFlowFlags & 0x400) {
                sp4C = 1;
            }

            if (D_800AE540.battleFlowFlags & 0x100) {
                sp4E = 2;
            }
        }
    }

    if (D_800AE540.battleFlowFlags & 0x40) {
        gCurrentGameState = STATE_TITLE_SCREEN;
    } else {
        gCurrentGameState = STATE_AREA_SELECT;
    }
}

void State_BattleNow(s32 arg0) {
    main_pool_push_state('QUIK');
    Team_BuildQuickBattleTeams(arg0);

    FRAGMENT_LOAD_AND_CALL(fragment63, 0, 0);
    FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

    main_pool_push_state('FIGT');

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

    main_pool_pop_state('FIGT');

    FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);

    main_pool_pop_state('QUIK');

    gCurrentGameState = STATE_MENU_SELECT;
}

void State_BattleFromEvent(void) {
    main_pool_push_state('EVNT');

    FRAGMENT_LOAD_AND_CALL(fragment63, 0, 0);
    FRAGMENT_LOAD_AND_CALL(fragment64, 0, 0);

    main_pool_push_state('FIGT');

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

    main_pool_pop_state('FIGT');

    FRAGMENT_LOAD_AND_CALL(fragment63, 2, 0);

    main_pool_pop_state('EVNT');

    gCurrentGameState = STATE_EVENT_BATTLE;
}

void State_FastN64Logo(void) {
    void (*frag_43_entry)(void);

    FRAGMENT_LOAD_AND_CALL(fragment35, 0, 0);

    main_pool_push_state('DOSE');

    frag_43_entry = FRAGMENT_LOAD(fragment43);
    frag_43_entry();

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD_AND_CALL(fragment75, 0, 0);

    gCurrentGameState = STATE_TITLE_SCREEN;

    main_pool_pop_state('DOSE');
}

void State_FastBattle(void) {
    main_pool_push_state('STAD');

    Team_BuildDeckOpponents(1, 0, 2, 0);
    Audio_PlayCommand(0xA0, 0xA, 0);

    FRAGMENT_LOAD(fragment34);
    FRAGMENT_LOAD_AND_CALL(fragment62, 0, &D_800AE540);

    main_pool_pop_state('STAD');

    gCurrentGameState = STATE_TITLE_SCREEN;
}

void func_8002B244(void) {
}

void State_KidsClubTitle(void) {
    MiniGame_Dispatch();
    gCurrentGameState = STATE_TITLE_SCREEN;
}

void Game_ShutdownAndLoadFragment(s32 arg0, s32 arg1) {
    osViBlack(1);
    GbTower_PowerOffAllPaks();
    Audio_FadeOutForShutdown();
    Game_ResetStorageDevice();
    Display_ClearFramebufferLine(1);
    Display_ApplyPendingVideoMode();
    Display_WaitForFrames(0xA);

    main_pool_pop_state('GAME');

    FRAGMENT_LOAD_AND_CALL(fragment32, arg0, arg1);
}

void Game_ResetStorageDevice(void) {
    Storage_ResetDisk();
}

void Game_Thread(UNUSED LEODiskID* arg) {
    s32 state;

    __osSetFpcCsr(0x01000C01U);

    Cont_SetupControllers();
    PokeIcon_StartThread();

    main_pool_push_state('GAME');

    D_800AE540.roundSelector = 0;

    // is this game connected to a Sticker Station?
    if (Controller_CheckAccessoryEeprom() != 0) {
        gCurrentGameState = STATE_FAST_N64_LOGO;
    }

    while (1) {
        state = Game_DoCopyProtection(gCurrentGameState);

        switch (state) {
            case STATE_N64_LOGO_INTRO:
                State_N64LogoIntro();
                break;
            case STATE_TITLE_SCREEN:
                State_TitleScreen();
                break;
            case STATE_N64DD_BOOT_UNUSED:
                State_N64ddBootUnused();
                break;
            case STATE_AREA_SELECT:
                State_AreaSelect();
                break;
            case STATE_GALLERY:
                State_Gallery();
                break;
            case STATE_EVENT_BATTLE:
                State_EventBattle();
                break;
            case STATE_OPTIONS:
                State_Options();
                break;
            case STATE_MENU_SELECT:
                State_MenuSelect();
                break;
            case STATE_STADIUM_MENU:
                State_StadiumMenu();
                break;
            case STATE_FREE_BATTLE:
                State_FreeBattle();
                break;
            case STATE_VS_MEWTWO:
                State_VsMewtwo();
                break;
            case STATE_KIDS_CLUB:
                State_KidsClub();
                break;
            case STATE_VICTORY_PALACE:
                State_VictoryPalace();
                break;
            case STATE_POKEMON_LAB:
                State_PokemonLab();
                break;
            case STATE_GB_TOWER:
                State_GbTower();
                break;
            case STATE_GYM_LEADER_CASTLE:
                State_GymLeaderCastle();
                break;
            case STATE_BATTLE_NOW_1P:
                State_BattleNow(1);
                break;
            case STATE_BATTLE_NOW_2P:
                State_BattleNow(2);
                break;
            case STATE_BATTLE_FROM_EVENT:
                State_BattleFromEvent();
                break;
            case STATE_STUBBED_DEBUG:
                State_StubbedDebug();
                break;
            case STATE_FAST_BATTLE:
                State_FastBattle();
                break;
            case STATE_KIDS_CLUB_TITLE:
                State_KidsClubTitle();
                break;
            case STATE_FAST_N64_LOGO:
                State_FastN64Logo();
                break;
            // same as state 1: N64 Logo + Intro.
            default:
                State_N64LogoIntro();
                break;
        }

        gLastGameState = state;
    }
}
