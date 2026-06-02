/*
 * DSP long compatibility — begin scope.
 * Legacy Windows DSP structs overlay float[] arrays and assume sizeof(long)==4.
 * On macOS LP64 (and any LP64 target), native long is 8 bytes, breaking the
 * struct/array overlay. Scoped '#define long int' forces 32-bit members for
 * the duration of the struct definition, matching the Windows LP32 layout.
 * Must be paired with dsp_long_compat_end.h at the end of the struct definition.
 */
#if defined(__APPLE__)
#pragma push_macro("long")
#define long int
#endif
