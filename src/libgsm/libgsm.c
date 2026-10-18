// PORT: C translation of the game's hand-written GSM 06.10 decoder kernels (libgsm.s; the announcer's voice).
//
// Same arithmetic as the MIPS code, including where it differs from the reference libgsm: 32-bit adds without
// saturation in the long- and short-term filters and in the de-emphasis, products truncated to 32 bits.
//
// State layout (gsm_state, see libgsm.h): s16 dp[160] at 0x000 (reconstructed long-term residual: 120 samples of
// history, then the current 40), s32 v[8] at 0x140 (short-term filter state, kept in registers s0-s7 by the
// assembly), s16 msr at 0x164 (de-emphasis), s16 nrp at 0x166, s16 LARpp[2][8] at 0x168, s16 frame_index at 0x188.
#include "libgsm.h"

#define GSM_DP(S) ((s16*)(S))
#define GSM_V(S) ((s32*)((u8*)(S) + 0x140))
#define GSM_MSR(S) (*(s16*)((u8*)(S) + 0x164))

// (a * b + 0x4000) >> 15 with the MIPS mult/mflo: the product's low 32 bits
static s32 MultR(s32 a, s32 b) {
    s32 p = (s32)(u32)((s64)a * (s64)b);
    return (s32)((u32)p + 0x4000) >> 15;
}

// xmaxc 0..7 -> exponent (high nibble, signed) and mantissa (low 3 bits)
static const s8 sXmaxcSmall[8] = { (s8)0xC7, (s8)0xD7, (s8)0xE3, (s8)0xE7, (s8)0xF1, (s8)0xF3, (s8)0xF5, (s8)0xF7 };
static const u16 sQLB[4] = { 0x0CCD, 0x2CCD, 0x5333, 0x7FFF };

// RPE decoding: inverse APCM of the 13 pulses (xMcr, overwritten with the decoded values) and their grid position
// in the 40-sample excitation erp.
void Gsm_RPE_Decoding(s16 xmaxcr, s16 Mcr, xMc* xMcr, s16* erp) {
    s16* xm = (s16*)xMcr;
    s32 exp, mant, i;
    s32 fac, shift, round;

    if (xmaxcr <= 7) {
        exp = sXmaxcSmall[xmaxcr & 7] >> 4;
        mant = sXmaxcSmall[xmaxcr & 7] & 7;
    } else {
        exp = (xmaxcr - 8) >> 3;
        mant = xmaxcr & 7;
    }
    fac = (mant << 11) + 0x47FF;
    shift = 6 - exp;
    round = (s32)(1u << ((shift - 1) & 31));
    for (i = 0; i < 13; i++) {
        s32 t = (s32)((u32)(s32)xm[i] << 13) - 0x7000;
        t = MultR(t, fac);
        t = (s32)((u32)t + (u32)round);
        xm[i] = (s16)(t >> (shift & 31));
    }
    for (i = 0; i < 40; i++) {
        erp[i] = 0;
    }
    for (i = 0; i < 13; i++) {
        erp[Mcr + 3 * i] = xm[i];
    }
}

// Long-term synthesis: shift the history by 40 and append erp + brp * dp[-Nr].
void Gsm_Long_term_synthesis_filtering(gsm_state* S, s16 Ncr, s32 bcr, s16* erp) {
    s16* dp = GSM_DP(S);
    s32 nr = S->nrp;
    s32 brp, k;

    if (Ncr >= 40 && Ncr <= 120) {
        nr = Ncr;
    }
    S->nrp = (s16)nr;
    brp = sQLB[bcr & 3];
    for (k = 0; k < 120; k++) {
        dp[k] = dp[k + 40];
    }
    for (k = 0; k < 40; k++) {
        s32 t = MultR(dp[120 - nr + k], brp);
        dp[120 + k] = (s16)(t + erp[k]);
    }
}

// LARp -> rp (after the interpolation step), as Gsm_Coefficients_40_159 in the assembly
static s16 LarpToRp(s32 t) {
    s32 neg = t < 0;
    s32 a;
    if (neg) {
        t = -t;
    }
    if (t < 11059) {
        a = t << 1;
    } else if (t < 20070) {
        a = t + 11059;
    } else {
        a = (t >> 2) + 26112;
    }
    if (a > 0x7FFF) {
        a = 0x7FFF;
    }
    return (s16)(neg ? -a : a);
}

static void Filter(gsm_state* S, const s16* rp, const s16* wt, s16* out, s32 count, s32* msrp) {
    s32* v = GSM_V(S);
    s32 msr = *msrp;
    s32 n, i;

    for (n = 0; n < count; n++) {
        s32 sri = wt[n];
        sri -= MultR(v[7], rp[7]);
        for (i = 6; i >= 0; i--) {
            sri -= MultR(v[i], rp[i]);
            v[i + 1] = MultR(rp[i], sri) + v[i];
        }
        v[0] = sri;
        // de-emphasis, then upscaling with saturation and truncation to 13 bits
        sri += MultR(0x6E14, msr);
        msr = sri;
        {
            s32 t = (s32)((u32)sri << 1);
            s32 o = ((t >> 15) + 1) >> 1;
            if (o != 0) {
                t = o < 0 ? -0x8000 : 0x7FFF;
            }
            out[n] = (s16)(t & 0xFFF8);
        }
    }
    *msrp = msr;
}

// Short-term synthesis of the 160 samples in wt into s, with the reflection coefficients interpolated between the
// previous and the current frame's LARs over the first 40 samples (13 / 14 / 13 samples), as in GSM 06.10.
void Gsm_Short_Term_Synthesis_Filter(gsm_state* S, s16* wt, s16* s) {
    const s16* cur = S->LARpp[S->frame_index != 0 ? 1 : 0];
    const s16* prev = S->LARpp[S->frame_index != 0 ? 0 : 1];
    s32 msr = GSM_MSR(S);
    s16 rp[8];
    s32 i;

    for (i = 0; i < 8; i++) {
        rp[i] = LarpToRp((cur[i] >> 2) + (prev[i] >> 1) + ((prev[i] >> 1) >> 1));
    }
    Filter(S, rp, wt, s, 13, &msr);
    for (i = 0; i < 8; i++) {
        rp[i] = LarpToRp((cur[i] >> 1) + (prev[i] >> 1));
    }
    Filter(S, rp, wt + 13, s + 13, 14, &msr);
    for (i = 0; i < 8; i++) {
        rp[i] = LarpToRp(((cur[i] >> 1) >> 1) + ((prev[i] >> 2) + (cur[i] >> 1)));
    }
    Filter(S, rp, wt + 27, s + 27, 13, &msr);
    for (i = 0; i < 8; i++) {
        rp[i] = LarpToRp(cur[i]);
    }
    Filter(S, rp, wt + 40, s + 40, 120, &msr);
    GSM_MSR(S) = (s16)msr;
}
