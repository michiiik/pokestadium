/*====================================================================
 *
 * Copyright 1993, Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics,
 * Inc.; the contents of this file may not be disclosed to third
 * parties, copied or duplicated in any form, in whole or in part,
 * without the prior written permission of Silicon Graphics, Inc.
 *
 * RESTRICTED RIGHTS LEGEND:
 * Use, duplication or disclosure by the Government is subject to
 * restrictions as set forth in subdivision (c)(1)(ii) of the Rights
 * in Technical Data and Computer Software clause at DFARS
 * 252.227-7013, and/or in similar or successor clauses in the FAR,
 * DOD or NASA FAR Supplement. Unpublished - rights reserved under the
 * Copyright Laws of the United States.
 *====================================================================*/

#include "n_synthInternals.h"
#include "../audio_stream.h"

// This function appears to be different than the stock n_alMainBusPull.

extern u8 gMusicFadeStepActive;
extern u8 gAudioProcessChannelsFlag;
extern s16* gMusicFadeBuffer;
extern u32 gMusicFadePos;

Acmd* n_alMainBusPull(s32 sampleOffset, Acmd* p) {
    Acmd* ptr = p;
    s32 i;

    aClearBuffer(ptr++, N_AL_MAIN_L_OUT, N_AL_DIVIDED << 1);

    ptr = (n_syn->mainBus->filter.handler)(sampleOffset, ptr);

    aMix(ptr++, 0, 0x7fff, N_AL_AUX_L_OUT, N_AL_MAIN_L_OUT);
    aMix(ptr++, 0, 0x7fff, N_AL_AUX_R_OUT, N_AL_MAIN_R_OUT);

    if (gMusicFadeStepActive != 0) {
        aClearBuffer(ptr++, 0x7C0, 0x2E0);
        n_aLoadBuffer(ptr++, 0x170, 0x7C0, osVirtualToPhysical(&gMusicFadeBuffer[gMusicFadePos]));
        n_aLoadBuffer(ptr++, 0x170, 0x930, osVirtualToPhysical(&gMusicFadeBuffer[gMusicFadePos]));

        gMusicFadePos += 0xB8;
        if (gMusicFadePos >= 0xB80) {
            gMusicFadePos = 0;
        }
        aMix(ptr++, 0, 0x7fff, N_AL_AUX_L_OUT, N_AL_MAIN_L_OUT);
        aMix(ptr++, 0, 0x7fff, N_AL_AUX_R_OUT, N_AL_MAIN_R_OUT);
        aClearBuffer(ptr++, 0x7C0, 0x2E0);
    }

    if (gAudioProcessChannelsFlag != 0) {
        for (i = 0; i < 2; i++) {
            aClearBuffer(ptr++, 0x7C0, 0x2E0);
            n_aLoadBuffer(ptr++, 0x170, 0x7C0, osVirtualToPhysical(gAudioChannelBuffer[i]->unk_000 + gAudioChannelReadPos[i]));
            n_aLoadBuffer(ptr++, 0x170, 0x930, osVirtualToPhysical(gAudioChannelBuffer[i]->unk_000 + gAudioChannelReadPos[i]));

            gAudioChannelReadPos[i] += 0xB8;
            if (gAudioChannelReadPos[i] >= 0x8A0) {
                gAudioChannelReadPos[i] = 0;
            }

            aMix(ptr++, 0, 0x7fff, N_AL_AUX_L_OUT, N_AL_MAIN_L_OUT);
            aMix(ptr++, 0, 0x7fff, N_AL_AUX_R_OUT, N_AL_MAIN_R_OUT);
        }
    }

    return ptr;
}
