/* macOS alias stubs for DSP process functions.
 * Comsftwr.c references the non-32 versions (dspsPeq8Process, dspsPlayProcess)
 * but we build with DSPSOFT_32_BIT so only the 32-bit versions are compiled.
 * These aliases forward to the 32-bit implementations.
 */
#if defined(__APPLE__) && defined(DSPSOFT_TARGET)

#include "codedefs.h"
#include "pt_defs.h"
#include "c_dsps.h"

/* Forward declarations of 32-bit versions */
void dspsPeq8Process32(long *lp_data, int l_length,
                       float *fp_params, float *fp_memory, float *fp_state,
                       struct hardwareMeterValType *sp_meters, int DSP_data_type);

void dspsPlayProcess32(long *lp_data, int l_length,
                       float *fp_params, float *fp_memory, float *fp_state,
                       struct hardwareMeterValType *sp_meters, int DSP_data_type);

/* Alias: dspsPeq8Process -> dspsPeq8Process32 */
void dspsPeq8Process(long *lp_data, int l_length,
                     float *fp_params, float *fp_memory, float *fp_state,
                     struct hardwareMeterValType *sp_meters, int DSP_data_type)
{
    dspsPeq8Process32(lp_data, l_length, fp_params, fp_memory, fp_state, sp_meters, DSP_data_type);
}

/* Alias: dspsPlayProcess -> dspsPlayProcess32 */
void dspsPlayProcess(long *lp_data, int l_length,
                     float *fp_params, float *fp_memory, float *fp_state,
                     struct hardwareMeterValType *sp_meters, int DSP_data_type)
{
    dspsPlayProcess32(lp_data, l_length, fp_params, fp_memory, fp_state, sp_meters, DSP_data_type);
}

#endif /* __APPLE__ && DSPSOFT_TARGET */
