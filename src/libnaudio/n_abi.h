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

#ifndef _N_ABI__
#define	_N_ABI__


/*
 * BEGIN C-specific section: (typedef's)
 */

#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)

#include <stdint.h>

/*
 * PORT: addresses in the command list are host pointers, which do not fit the 24/32-bit fields. Every command that
 * carries one records the full pointer in a side table keyed by the command's slot (Port_AcmdAddr, audio_hle.cpp);
 * the field keeps the low bits. PORT_ACMD_BE marks DRAM data in N64 byte order (sample data DMA'd from ROM); the
 * rest (state, delay lines, the CPU-written stream buffers, the output) holds host-order 16-bit samples.
 */
#define PORT_ACMD_NATIVE 0
#define PORT_ACMD_BE 1
unsigned int Port_AcmdAddr(void* cmd, uintptr_t addr, int kind);
#define PORT_ACMD_ADDR(pkt, s, kind) Port_AcmdAddr((void*)(pkt), (uintptr_t)(s), kind)

/*
 * Macros to assemble the audio command list
 */

#define	n_aADPCMdec(pkt, s, f, c, a, d)					\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_ADPCM, 24, 8) | _SHIFTL(PORT_ACMD_ADDR(_a, s, PORT_ACMD_NATIVE), 0, 24));   \
	_a->words.w1 = (_SHIFTL(f, 28, 4) | _SHIFTL(c, 16, 12) |        \
			_SHIFTL(a, 12, 4) | _SHIFTL(d, 0, 12));         \
}

#define n_aPoleFilter(pkt, f, g, t, s)					\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_POLEF, 24, 8) | _SHIFTL(f, 16, 8) |	\
			_SHIFTL(g, 0, 16)); 				\
	_a->words.w1 = (_SHIFTL(t, 24, 8) |                             \
			_SHIFTL(PORT_ACMD_ADDR(_a, s, PORT_ACMD_NATIVE), 0, 24));		\
}

#define n_aEnvMixer(pkt, f, t, s)						\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_ENVMIXER, 24, 8) | _SHIFTL(f, 16, 8) |\
			_SHIFTL(t, 0, 16));                     	\
	_a->words.w1 = PORT_ACMD_ADDR(_a, s, PORT_ACMD_NATIVE);				\
}

#define n_aInterleave(pkt)						\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = _SHIFTL(A_INTERLEAVE, 24, 8);    		\
	_a->words.w1 = 0;    		\
}

#define n_aLoadBufferKind(pkt, c, d, s, kind)				\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_LOADBUFF, 24, 8) | _SHIFTL(c, 12, 12)|\
			_SHIFTL(d, 0, 12));                             \
	_a->words.w1 = PORT_ACMD_ADDR(_a, s, kind);				\
}
#define n_aLoadBuffer(pkt, c, d, s) n_aLoadBufferKind(pkt, c, d, s, PORT_ACMD_NATIVE)

#define n_aResample(pkt, s, f, p, i, o)					\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_RESAMPLE, 24, 8) | _SHIFTL(PORT_ACMD_ADDR(_a, s, PORT_ACMD_NATIVE), 0, 24));\
	_a->words.w1 = (_SHIFTL(f, 30, 2) | _SHIFTL(p, 14, 16) |        \
			_SHIFTL(i, 2, 12) | _SHIFTL(o, 0, 2));          \
}

#define n_aSaveBuffer(pkt, c, d, s)					\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_SAVEBUFF, 24, 8) | _SHIFTL(c, 12, 12)|\
			_SHIFTL(d, 0, 12));                             \
	_a->words.w1 = PORT_ACMD_ADDR(_a, s, PORT_ACMD_NATIVE);				\
}

#define n_aSetVolume(pkt, f, v, t, r)					\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = (_SHIFTL(A_SETVOL, 24, 8) | _SHIFTL(f, 16, 8) |	\
			_SHIFTL(v, 0, 16)); 				\
	_a->words.w1 = _SHIFTL(t, 16, 16) | _SHIFTL(r, 0, 16);		\
}

#define n_aLoadADPCM(pkt, c, d)						\
{									\
	Acmd *_a = (Acmd *)pkt;						\
									\
	_a->words.w0 = _SHIFTL(A_LOADADPCM, 24, 8) | _SHIFTL(c, 0, 24);	\
        _a->words.w1 = PORT_ACMD_ADDR(_a, d, PORT_ACMD_NATIVE);                                \
}

/* PORT: the libultra (PR/abi.h) commands with addresses that the n_audio code uses */
#undef aLoadADPCM
#define aLoadADPCM(pkt, c, d) n_aLoadADPCM(pkt, c, d)
#undef aSetLoop
#define aSetLoop(pkt, a)                                                \
{                                                                       \
        Acmd *_a = (Acmd *)pkt;                                         \
        _a->words.w0 = _SHIFTL(A_SETLOOP, 24, 8);                       \
        _a->words.w1 = PORT_ACMD_ADDR(_a, a, PORT_ACMD_NATIVE);         \
}

#endif /* _LANGUAGE_C */

#endif /*  !_N_ABI__  */







