// macOS stub for <mmsystem.h> — Windows Multimedia System header.
#pragma once
#if defined(__APPLE__)
#include "pt_mac_compat.h"

// WAVEFORMATEX stub
typedef struct {
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
} WAVEFORMATEX;

#define WAVE_FORMAT_PCM 1
#define WAVE_FORMAT_IEEE_FLOAT 3

#endif // __APPLE__
