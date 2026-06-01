// macOS stub implementations for DSP hardware communication functions.
// These are never called in the software DSP path (DSPSOFT_TARGET) but
// the linker needs them because the COM layer compiles with DSPFX1 defined.
#if defined(__APPLE__)

#include "codedefs.h"

extern "C" {
#include "hrdwr.h"
}

extern "C" {

// comHrdwr.c stubs
int COMHRDWR_DECL comHrdwrPutChar(int, long) { return OKAY; }
int COMHRDWR_DECL comHrdwrWriteParam(int, long, long) { return OKAY; }
int COMHRDWR_DECL comHrdwrGetChar(int, long *lp_val) { if (lp_val) *lp_val = 0; return OKAY; }
int COMHRDWR_DECL comHrdwrWaitForFlag(int) { return OKAY; }
int COMHRDWR_DECL comHrdwrCheckFlag(int, int *ip_flag) { if (ip_flag) *ip_flag = 0; return OKAY; }
int COMHRDWR_DECL comHrdwrProcessWaveBuffer(int, long *, long, int, int) { return OKAY; }
int COMHRDWR_DECL comHrdwrProcessActiveBuffer(int, short *, long, int, int) { return OKAY; }

// comHrdut.c stubs
int COMHRDWR_DECL comHrdwrInitializeCommHrdwr(int *) { return OKAY; }
int COMHRDWR_DECL comHrdwrResetCommHrdwr(int *) { return OKAY; }
int COMHRDWR_DECL comHrdwrLdcharBuf(unsigned short, unsigned short, long *) { return OKAY; }

// hrdwrsc.c stubs
int COMHRDWR_DECL hrdwrResetProcessor(unsigned short) { return OKAY; }
int COMHRDWR_DECL hrdwrReadXferReg(unsigned short, long *lp) { if (lp) *lp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrReadXferRegIfNotBusy(unsigned short, short *sp, long *lp) { if (sp) *sp = 0; if (lp) *lp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrReadXferRegIfFull(unsigned short, long *lp, short *sp) { if (lp) *lp = 0; if (sp) *sp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrReadXferRegIfFullAndNotBusy(unsigned short, short *sp1, long *lp, short *sp2) { if (sp1) *sp1 = 0; if (lp) *lp = 0; if (sp2) *sp2 = 0; return OKAY; }
int COMHRDWR_DECL hrdwrWaitForXferRegFull(unsigned short, long *lp) { if (lp) *lp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrWriteXferReg(unsigned short, long) { return OKAY; }
int COMHRDWR_DECL hrdwrWriteXferRegIfNotBusy(unsigned short, long, short *sp) { if (sp) *sp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrWriteXferRegIfEmpty(unsigned short, long, short *sp) { if (sp) *sp = 1; return OKAY; }
int COMHRDWR_DECL hrdwrWriteXferRegIfEmptyAndNotBusy(unsigned short, long, short *sp1, short *sp2) { if (sp1) *sp1 = 0; if (sp2) *sp2 = 1; return OKAY; }
int COMHRDWR_DECL hrdwrWriteParameterIfEmpty(unsigned short, long, long, short *sp) { if (sp) *sp = 1; return OKAY; }
int COMHRDWR_DECL hrdwrWriteParameterIfEmptyAndNotBusy(unsigned short, long, long, short *sp1, short *sp2) { if (sp1) *sp1 = 0; if (sp2) *sp2 = 1; return OKAY; }
int COMHRDWR_DECL hrdwrWriteCommandReg(unsigned short, unsigned short) { return OKAY; }
int COMHRDWR_DECL hrdwrWriteCommandRegIfNotBusy(unsigned short, unsigned short, short *sp) { if (sp) *sp = 0; return OKAY; }
int COMHRDWR_DECL hrdwrReadStatusReg(unsigned short, unsigned short *up) { if (up) *up = 0; return OKAY; }
int COMHRDWR_DECL hrdwrReadStatusRegIfNotBusy(unsigned short, short *sp, unsigned short *up) { if (sp) *sp = 0; if (up) *up = 0; return OKAY; }
int COMHRDWR_DECL hrdwrGetDspFlag(unsigned short, unsigned short *up) { if (up) *up = 0; return OKAY; }
int COMHRDWR_DECL hrdwrWaitStat(unsigned short, unsigned short, unsigned short) { return OKAY; }
int COMHRDWR_DECL hrdwrDMAtransmitStereo(unsigned short, long *, long) { return OKAY; }
int COMHRDWR_DECL hrdwrDMAtransmitMono(unsigned short, long *, long) { return OKAY; }
int COMHRDWR_DECL hrdwrDMAreceiveStereo(unsigned short, long *, long) { return OKAY; }
int COMHRDWR_DECL hrdwrDMAreceiveMono(unsigned short, long *, long) { return OKAY; }

// EEPROM stubs (referenced from Comeprom.cpp)
int comHrdEepromReadUlong(unsigned short, unsigned long *ulp) { if (ulp) *ulp = 0; return OKAY; }
int comHrdEepromWriteUlong(unsigned short, unsigned long) { return OKAY; }

} // extern "C"

#endif // __APPLE__
