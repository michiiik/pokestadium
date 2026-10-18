#include "global.h"

s32 osGbSetNextBuffer(void* bufPtr, u32 size) {
  static u8 hdwrBugFlag = FALSE;
  char* bptr;
  s32 status;

#if BUILD_VERSION >= VERSION_J
  if (__osAiDeviceBusy()) {
      return -1;
  }
#endif

#ifdef _DEBUG
  if ((u32)bufPtr & (8 - 1)) {
      __osError(ERR_OSAISETNEXTBUFFER_ADDR, 1, bufPtr);
      return -1;
  }

  if ((u32)size & (8 - 1)) {
      __osError(ERR_OSAISETNEXTBUFFER_SIZE, 1, size);
      return -1;
  }
#endif

  bptr = bufPtr;

  if (hdwrBugFlag) {
      bptr = (u8*)bufPtr - 0x2000;
  }

  if ((((uintptr_t)bufPtr + size) & 0x1fff) == 0) {
      hdwrBugFlag = TRUE;
  } else {
      hdwrBugFlag = FALSE;
  }

#if BUILD_VERSION < VERSION_J
  status = IO_READ(AI_STATUS_REG);
  //! @bug The __osAiDeviceBusy call should be above the hardware bug workaround to ensure that it was only
  //! performed when a transfer was guaranteed to start. If this condition passes and this function returns without
  //! submitting a buffer for DMA, the code above will lose track of when to apply the workaround.
  if (status & AI_STATUS_FIFO_FULL) {
      return -1;
  }
#endif

  IO_WRITE(AI_DRAM_ADDR_REG, OS_PHYSICAL_TO_K0(bptr));
  IO_WRITE(AI_LEN_REG, size);
  {
      // PORT: the audio interface is the host's sound output (src/port/gb_audio_out.cpp)
      extern void Port_GbAudioSubmit(const void* samples, u32 bytes);
      Port_GbAudioSubmit(bufPtr, size);
  }
  return 0;
}
