#include "particle_math.h"

void Particle_UpdateFrameCounters(void) {
    gParticleFrameCounter++;
    gParticleFrameMod64 = gParticleFrameCounter & 0x3F;
    gParticleFrameMod32 = gParticleFrameCounter & 0x1F;
    gParticleFrameMod16 = gParticleFrameCounter & 0xF;
    gParticleFrameMod8 = gParticleFrameCounter & 7;
    gParticleFrameMod4 = gParticleFrameCounter & 3;
    gParticleFrameMod3 = gParticleFrameCounter % 3;
    gParticleFrameMod2 = gParticleFrameCounter & 1;
    gParticleFrameDiv64 = gParticleFrameCounter >> 6;
    gParticleFrameDiv32 = gParticleFrameCounter >> 5;
    gParticleFrameDiv16 = gParticleFrameCounter >> 4;
    gParticleFrameDiv8 = gParticleFrameCounter >> 3;
    gParticleFrameDiv4 = gParticleFrameCounter >> 2;
    gParticleFrameDiv3 = gParticleFrameCounter / 3;
    gParticleFrameDiv2 = gParticleFrameCounter >> 1;
    gParticleFrameDiv64Parity = gParticleFrameDiv64 & 1;
    gParticleFrameDiv32Parity = gParticleFrameDiv32 & 1;
    gParticleFrameDiv16Parity = gParticleFrameDiv16 & 1;
    gParticleFrameDiv8Parity = gParticleFrameDiv8 & 1;
    gParticleFrameDiv4Parity = gParticleFrameDiv4 & 1;
    gParticleFrameDiv3Parity = gParticleFrameDiv3 & 1;
    gParticleFrameDiv2Parity = gParticleFrameDiv2 & 1;
}

void Particle_UpdateFrameCountersAlias(void) {
    Particle_UpdateFrameCounters();
}
