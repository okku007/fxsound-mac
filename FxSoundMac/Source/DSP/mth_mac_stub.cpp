// macOS stub implementations for mth (math utility) functions that are
// guarded by #if defined(WIN32) in MthUtil.cpp.
#if defined(__APPLE__)

#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "codedefs.h"
#include "slout.h"
#include "mth.h"

/*
 * mthIsLong_Wide() — check if wide string represents a long integer
 */
int PT_DECLSPEC mthIsLong_Wide(wchar_t *wcp_string, int *ip_is_long)
{
    int length;
    int index;
    int done;

    *ip_is_long = IS_FALSE;

    if (wcp_string == NULL)
        return OKAY;

    length = (int)wcslen(wcp_string);
    if (length <= 0)
        return OKAY;

    done = IS_FALSE;
    index = 0;

    while (!done)
    {
        if ((wcp_string[index] < L'0') || (wcp_string[index] > L'9'))
        {
            if (index != 0)
                done = IS_TRUE;
            else if (wcp_string[index] != L'-')
                done = IS_TRUE;
        }

        if (!done)
        {
            index++;
            if (index == length)
            {
                done = IS_TRUE;
                *ip_is_long = IS_TRUE;
            }
        }
    }

    return OKAY;
}

/*
 * mthIsLong() — char version
 */
int PT_DECLSPEC mthIsLong(char *cp_string, int *ip_is_long)
{
    wchar_t wcp_string[512];
    if (ip_is_long) *ip_is_long = IS_FALSE;
    if (cp_string == NULL) return OKAY;
    mbstowcs(wcp_string, cp_string, 512);
    return mthIsLong_Wide(wcp_string, ip_is_long);
}

/*
 * mthIsHex_Wide() — check if wide string represents a hex value
 */
int PT_DECLSPEC mthIsHex_Wide(wchar_t *wcp_string, int *ip_is_hex)
{
    if (ip_is_hex) *ip_is_hex = IS_FALSE;
    if (wcp_string == NULL) return OKAY;
    int len = (int)wcslen(wcp_string);
    if (len == 0) return OKAY;
    int start = 0;
    if (wcp_string[0] == L'0' && len > 2 && (wcp_string[1] == L'x' || wcp_string[1] == L'X'))
        start = 2;
    for (int i = start; i < len; i++) {
        wchar_t c = wcp_string[i];
        if (!((c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F')))
            return OKAY;
    }
    if (ip_is_hex) *ip_is_hex = IS_TRUE;
    return OKAY;
}

/*
 * mthIsHex() — char version
 */
int PT_DECLSPEC mthIsHex(char *cp_string, int *ip_is_hex)
{
    wchar_t wcp_string[512];
    if (ip_is_hex) *ip_is_hex = IS_FALSE;
    if (cp_string == NULL) return OKAY;
    mbstowcs(wcp_string, cp_string, 512);
    return mthIsHex_Wide(wcp_string, ip_is_hex);
}

/*
 * mthRotN() — ROT-N cipher for char strings
 */
int PT_DECLSPEC mthRotN(char *cp_string, int i_rot_n, int i_length)
{
    if (cp_string == NULL) return OKAY;
    for (int i = 0; i < i_length; i++) {
        if (cp_string[i] >= 'a' && cp_string[i] <= 'z')
            cp_string[i] = (char)('a' + (cp_string[i] - 'a' + i_rot_n) % 26);
        else if (cp_string[i] >= 'A' && cp_string[i] <= 'Z')
            cp_string[i] = (char)('A' + (cp_string[i] - 'A' + i_rot_n) % 26);
    }
    return OKAY;
}

/*
 * mthRotN_Wide() — ROT-N cipher for wide strings
 */
int PT_DECLSPEC mthRotN_Wide(wchar_t *wcp_string, int i_rot_n, int i_length)
{
    if (wcp_string == NULL) return OKAY;
    for (int i = 0; i < i_length; i++) {
        if (wcp_string[i] >= L'a' && wcp_string[i] <= L'z')
            wcp_string[i] = (wchar_t)(L'a' + (wcp_string[i] - L'a' + i_rot_n) % 26);
        else if (wcp_string[i] >= L'A' && wcp_string[i] <= L'Z')
            wcp_string[i] = (wchar_t)(L'A' + (wcp_string[i] - L'A' + i_rot_n) % 26);
    }
    return OKAY;
}

#endif // __APPLE__
