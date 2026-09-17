/*====================================================================
 * GSM 06.10 full-rate speech transcoder
 *====================================================================*/

#ifndef _LIBGSM_H_
#define _LIBGSM_H_

#include "global.h"

typedef struct gsm_state {
    /* 0x000 */ char pad0[0x166];
    /* 0x166 */ s16 nrp;
    /* 0x168 */ s16 LARpp[2][8];
    /* 0x188 */ s16 frame_index;
} gsm_state; // size = 0x18A

typedef struct xMc {
    s16 xRotA;
    s16 xRotB;
    s16 flag0;
    s16 yRotA;
    s16 yRotB;
    s16 flag1;
    s16 zRotA;
    s16 zRotB;
    s16 flag2;
    s16 unkA;
    s16 unkB;
    s16 unkC;
    s16 unkD;
} xMc; // size 0x1A

// libgsm.s
void Gsm_Short_Term_Synthesis_Filter(gsm_state*, s16*, s16*);
void Gsm_RPE_Decoding(s16*, s16*, xMc*, s16*);
void Gsm_Long_term_synthesis_filtering(gsm_state*, s16, s32, s16*);

#endif // _LIBGSM_H_
