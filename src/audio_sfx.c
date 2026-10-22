#include "audio_sfx.h"
#include "src/libnumus/player.h"
#include "src/audio_sfx.h"
#include "src/audio_commands_category2.h"
#include "src/audio_commands.h"
#include "src/audio_data.h"

static s32 gLastSfxHandle = 0;
static s32 scroll_sfx = 0;
static s32 back_sfx = 0;
static s32 confirm_sfx = 0;
static s32 gSfxHandle_MenuScreenOpen = 0;
static s32 gSfxHandle_MenuError = 0;
static s32 gSfxHandle_GalleryCameraShutter = 0;
static s32 gSfxHandle_GalleryCameraExit = 0;
static u8 gSfx3CCooldown = 0;

s32 Audio_PlaySoundEffectById(u32 sfx) {
    u32 temp_v0_3;
    s32 sp28;
    f32 sp24;

    temp_v0_3 = sfx & 0xFFFF0000;
    sp28 = 0x80;
    sp24 = 0.0f;

    if (temp_v0_3 == 0x01200000) {
        return Audio_PlayCategory12SoundCommand(sfx, 0, 0);
    }

    if (temp_v0_3 == 0x01100000) {
        return Audio_PlayCategory11SoundCommand(sfx, 0, 0);
    }

    if (sfx == SFX_TITLE_START) {
        if (!(gAudioDMAFlags & 0x80000000)) {
            return 0;
        }
        gLastSfxHandle = Audio_PlaySoundEffect(gSfxBankSpecial, gSfxTable, SFX_TITLE_START, 0x70, 0x80, -1);
        Audio_QueueFadeSoundCommand(gMusicHandle, 0x28);
        return gLastSfxHandle;
    }

    if (sfx == 0x30) {
        sfx = 0x26;
    }

    if (sfx == SFX_CAMERA_TEXT) {
        if (gSfx3CCooldown != 0) {
            gSfx3CCooldown++;
            if (gSfx3CCooldown > 0) {
                gSfx3CCooldown = 0;
            }
            return 0;
        }
        gSfx3CCooldown++;
        if (1) {}
        sfx = 5;
        sp24 = -12.0f;
        sp28 = 0x38;
    }

    switch (sfx) {
        case SFX_MENU_SCROLL:
            Audio_QueueFadeSoundCommand(scroll_sfx, 1);
            break;

        case 0x5:
        case 0x6:
        case 0x7:
        case 0x9:
        case 0xD:
        case 0xE:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case SFX_MOVE_SWAP:
        case SFX_ZOOM_IN:
        case SFX_ZOOM_OUT:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case SFX_PAUSE:
        case SFX_RESUME:
        case SFX_PAK_CURSOR:
        case 0x26:
        case SFX_PAGE_CHANGE:
        case SFX_TROPHY_APPEAR:
        case 0x29:
        case 0x2F:
            break;

        case SFX_MENU_CONFIRM:
            Audio_QueueFadeSoundCommand(confirm_sfx, 1);
            break;

        case SFX_MENU_BACK:
            Audio_QueueFadeSoundCommand(back_sfx, 1);
            break;

        case SFX_PANEL_OPEN:
            Audio_QueueFadeSoundCommand(gSfxHandle_MenuScreenOpen, 0x14);
            break;

        case SFX_MENU_ERROR:
            Audio_QueueFadeSoundCommand(gSfxHandle_MenuError, 2);
            break;

        case SFX_PHOTO_CAPTURE:
            Audio_QueueFadeSoundCommand(gSfxHandle_GalleryCameraShutter, 2);
            sp28 = 0xA0;
            break;

        case SFX_CAMERA_FULL:
            Audio_QueueFadeSoundCommand(gSfxHandle_GalleryCameraShutter, 2);
            break;

        case SFX_CAMERA_EXIT:
            Audio_QueueFadeSoundCommand(gSfxHandle_GalleryCameraExit, 2);
            break;

        case SFX_N64_LOGO:
            sp28 = 0xE0;
            break;

        case 0x31:
            sfx -= 1;
            break;

        case 0xB:
        case 0xC:
            sp28 = 0xD0;
            break;

        case SFX_PAK_LAB_SELECT:
        case SFX_PAK_TOWER_SELECT:
            Audio_PlaySoundEffectById(0x1B);
            break;

        case SFX_AREA_CURSOR:
            gLastSfxHandle = Audio_PlayDirectSoundCommand(0x01000001, 0, 0);
            return gLastSfxHandle;

        case 0x33:
            gLastSfxHandle = Audio_PlayDirectSoundCommand(0x01000002, 0, 0);
            return gLastSfxHandle;

        default:
            return 0;
    }

    gLastSfxHandle = Audio_PlaySoundEffect(gSfxBankStandard, gSfxTable, sfx, sp28, 0x80, -1);
    if (sp24 != 0.0f) {
        Audio_QueueVoiceDetune(gLastSfxHandle, sp24);
    }

    switch (sfx) {
        case SFX_MENU_SCROLL:
            scroll_sfx = gLastSfxHandle;
            break;

        case SFX_MENU_CONFIRM:
            confirm_sfx = gLastSfxHandle;
            break;

        case SFX_MENU_BACK:
            back_sfx = gLastSfxHandle;
            break;

        case SFX_PANEL_OPEN:
            gSfxHandle_MenuScreenOpen = gLastSfxHandle;
            break;

        case SFX_MENU_ERROR:
            gSfxHandle_MenuError = gLastSfxHandle;
            break;

        case SFX_PHOTO_CAPTURE:
            gSfxHandle_GalleryCameraShutter = gLastSfxHandle;
            break;

        case SFX_CAMERA_FULL:
            gSfxHandle_GalleryCameraExit = gLastSfxHandle;
            break;
    }

    return gLastSfxHandle;
}
