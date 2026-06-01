// macOS stub implementations for Windows Registry functions.
// All registry operations are no-ops on macOS — settings are not persisted
// via the registry. The DSP layer uses registry for session state only;
// on macOS this state is simply not persisted between runs.
#if defined(__APPLE__)

#include "codedefs.h"
#include "reg.h"

int PT_DECLSPEC regReadTopDir_Wide(wchar_t *, int, int, int, CSlout *) { return OKAY; }
int PT_DECLSPEC regReadRegisteredOwner(char *, int) { return OKAY; }
int PT_DECLSPEC regReadRegisteredOwner_Wide(wchar_t *, int) { return OKAY; }
int PT_DECLSPEC regRemoveKey(int, char *) { return OKAY; }
int PT_DECLSPEC regRemoveKey_Wide(int, wchar_t *) { return OKAY; }
int PT_DECLSPEC regCreateKey(int, char *, char *) { return OKAY; }
int PT_DECLSPEC regCreateKey_Wide(int, wchar_t *, wchar_t *) { return OKAY; }
int PT_DECLSPEC regReadKey(int, char *, int *ip_exists, char *cp_value, unsigned long) {
    if (ip_exists) *ip_exists = 0;
    if (cp_value) cp_value[0] = '\0';
    return OKAY;
}
int PT_DECLSPEC regReadKey_Wide(int, wchar_t *, int *ip_exists, wchar_t *wcp_value, unsigned long) {
    if (ip_exists) *ip_exists = 0;
    if (wcp_value) wcp_value[0] = L'\0';
    return OKAY;
}
int PT_DECLSPEC regCreateKeyTest_Wide(int, wchar_t *, wchar_t *, int *ip_success) {
    if (ip_success) *ip_success = 1;
    return OKAY;
}
int PT_DECLSPEC regCreateKeyWithKeyname_Dword_Wide(int, wchar_t *, wchar_t *, unsigned long) { return OKAY; }
int PT_DECLSPEC regCreateKeyWithKeyname_String_Wide(int, wchar_t *, wchar_t *, wchar_t *) { return OKAY; }
int PT_DECLSPEC regReadKeyWithKeyname_String_Wide(int, wchar_t *, wchar_t *, int *ip_exists, wchar_t *wcp_value, unsigned long) {
    if (ip_exists) *ip_exists = 0;
    if (wcp_value) wcp_value[0] = L'\0';
    return OKAY;
}
int PT_DECLSPEC regReadKeyWithKeyname_Dword_Wide(int, wchar_t *, wchar_t *, int *ip_exists, unsigned long *ulp_value) {
    if (ip_exists) *ip_exists = 0;
    if (ulp_value) *ulp_value = 0;
    return OKAY;
}
int PT_DECLSPEC regRecursiveDeleteFolder_Wide(int, wchar_t *) { return OKAY; }

#endif // __APPLE__
