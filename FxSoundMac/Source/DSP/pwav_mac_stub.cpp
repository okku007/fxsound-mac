// macOS stub implementations for PWAV functions needed by the COM layer.
// The full PWAV library is Windows-specific (HWAVEIN, HWAVEOUT, HMMIO, etc.).
// Only the format conversion functions are needed for the software DSP path.
#if defined(__APPLE__)

#include <stdint.h>
#include "codedefs.h"
#include "Pwav.h"

/*
 * pwav24BitToFloat() — convert 24-bit packed samples to float
 * 24-bit samples are stored as 3 bytes, little-endian, sign-extended.
 */
int PT_DECLSPEC pwav24BitToFloat(char *cp_24bit_buf, float *fp_float_buf, int i_num_samples, int i_stereo)
{
    if (cp_24bit_buf == NULL || fp_float_buf == NULL) return OKAY;
    int channels = i_stereo ? 2 : 1;
    int total = i_num_samples * channels;
    for (int i = 0; i < total; i++) {
        unsigned char *p = (unsigned char *)cp_24bit_buf + i * 3;
        int32_t val = ((int32_t)p[0]) | ((int32_t)p[1] << 8) | ((int32_t)p[2] << 16);
        // Sign extend from 24 bits
        if (val & 0x800000) val |= (int32_t)0xFF000000;
        fp_float_buf[i] = (float)val / (float)0x800000;
    }
    return OKAY;
}

/*
 * pwavFloatTo24Bit() — convert float samples to 24-bit packed
 */
int PT_DECLSPEC pwavFloatTo24Bit(float *fp_float_buf, char *cp_24bit_buf, int i_num_samples, int i_stereo)
{
    if (fp_float_buf == NULL || cp_24bit_buf == NULL) return OKAY;
    int channels = i_stereo ? 2 : 1;
    int total = i_num_samples * channels;
    for (int i = 0; i < total; i++) {
        float f = fp_float_buf[i];
        if (f > 1.0f) f = 1.0f;
        if (f < -1.0f) f = -1.0f;
        int32_t val = (int32_t)(f * (float)0x7FFFFF);
        unsigned char *p = (unsigned char *)cp_24bit_buf + i * 3;
        p[0] = (unsigned char)(val & 0xFF);
        p[1] = (unsigned char)((val >> 8) & 0xFF);
        p[2] = (unsigned char)((val >> 16) & 0xFF);
    }
    return OKAY;
}

#endif // __APPLE__
