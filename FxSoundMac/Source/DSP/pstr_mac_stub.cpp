// macOS stub implementations for pstr functions needed by the DSP layer.
// Only implements the functions actually referenced by the DSP sources.
#if defined(__APPLE__)

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <strings.h>  // for strcasecmp
#include <wctype.h>   // for iswdigit
#include "codedefs.h"
#include "slout.h"
#include "pstr.h"

/*
 * pstrCalcLocationOfStrInStr_Wide()
 * Find wcp_search_str within wcp_original_str.
 * Returns ip_found=IS_TRUE and ip_location (0-based) if found.
 */
int PT_DECLSPEC pstrCalcLocationOfStrInStr_Wide(wchar_t *wcp_original_str,
                                                  wchar_t *wcp_search_str,
                                                  int i_case_sensitive,
                                                  int *ip_found,
                                                  int *ip_location)
{
    if (ip_found) *ip_found = IS_FALSE;
    if (ip_location) *ip_location = 0;

    if (wcp_original_str == NULL || wcp_search_str == NULL)
        return OKAY;

    const wchar_t *found = NULL;
    if (i_case_sensitive) {
        found = wcsstr(wcp_original_str, wcp_search_str);
    } else {
        // Case-insensitive wide string search
        size_t orig_len = wcslen(wcp_original_str);
        size_t search_len = wcslen(wcp_search_str);
        if (search_len > orig_len) return OKAY;
        for (size_t i = 0; i <= orig_len - search_len; i++) {
            if (wcsncasecmp(wcp_original_str + i, wcp_search_str, search_len) == 0) {
                found = wcp_original_str + i;
                break;
            }
        }
    }

    if (found) {
        if (ip_found) *ip_found = IS_TRUE;
        if (ip_location) *ip_location = (int)(found - wcp_original_str);
    }
    return OKAY;
}

/*
 * pstrCalcStrLength_Wide() — return length of wide string
 */
int PT_DECLSPEC pstrCalcStrLength_Wide(wchar_t *wcp_str, int *ip_length)
{
    if (ip_length) *ip_length = wcp_str ? (int)wcslen(wcp_str) : 0;
    return OKAY;
}

/*
 * pstrCalcStrLength() — return length of char string
 */
int PT_DECLSPEC pstrCalcStrLength(char *cp_str, int *ip_length)
{
    if (ip_length) *ip_length = cp_str ? (int)strlen(cp_str) : 0;
    return OKAY;
}

/*
 * pstrTruncateAtFirstOccurrenceOfChar_Wide()
 */
int PT_DECLSPEC pstrTruncateAtFirstOccurrenceOfChar_Wide(wchar_t *wcp_str, wchar_t wc_char)
{
    if (wcp_str == NULL) return OKAY;
    wchar_t *p = wcschr(wcp_str, wc_char);
    if (p) *p = L'\0';
    return OKAY;
}

/*
 * pstrRemoveAllOccurrencesOfChar_Wide()
 */
int PT_DECLSPEC pstrRemoveAllOccurrencesOfChar_Wide(wchar_t *wcp_str, wchar_t wc_char)
{
    if (wcp_str == NULL) return OKAY;
    wchar_t *src = wcp_str, *dst = wcp_str;
    while (*src) {
        if (*src != wc_char) *dst++ = *src;
        src++;
    }
    *dst = L'\0';
    return OKAY;
}

/*
 * pstrCopy_Wide()
 */
int PT_DECLSPEC pstrCopy_Wide(wchar_t *wcp_dest, wchar_t *wcp_src, int i_max_len)
{
    if (wcp_dest == NULL || wcp_src == NULL) return OKAY;
    wcsncpy(wcp_dest, wcp_src, (size_t)i_max_len);
    wcp_dest[i_max_len - 1] = L'\0';
    return OKAY;
}

/*
 * pstrCopy()
 */
int PT_DECLSPEC pstrCopy(char *cp_dest, char *cp_src, int i_max_len)
{
    if (cp_dest == NULL || cp_src == NULL) return OKAY;
    strncpy(cp_dest, cp_src, (size_t)i_max_len);
    cp_dest[i_max_len - 1] = '\0';
    return OKAY;
}

/*
 * pstrCalcNumOccurrencesOfChar_Wide()
 */
int PT_DECLSPEC pstrCalcNumOccurrencesOfChar_Wide(wchar_t *wcp_str, wchar_t wc_char, int *ip_count)
{
    if (ip_count) *ip_count = 0;
    if (wcp_str == NULL) return OKAY;
    int count = 0;
    while (*wcp_str) { if (*wcp_str++ == wc_char) count++; }
    if (ip_count) *ip_count = count;
    return OKAY;
}

/*
 * pstrIsAllDigits_Wide()
 */
int PT_DECLSPEC pstrIsAllDigits_Wide(wchar_t *wcp_str, int *ip_all_digits)
{
    if (ip_all_digits) *ip_all_digits = IS_FALSE;
    if (wcp_str == NULL || *wcp_str == L'\0') return OKAY;
    int all = IS_TRUE;
    while (*wcp_str) { if (!iswdigit(*wcp_str++)) { all = IS_FALSE; break; } }
    if (ip_all_digits) *ip_all_digits = all;
    return OKAY;
}

/*
 * pstrGetSubString_Wide() — extract substring
 */
int PT_DECLSPEC pstrGetSubString_Wide(wchar_t *wcp_original, int i_start, int i_length,
                                       wchar_t *wcp_result, int i_result_buf_size)
{
    if (wcp_original == NULL || wcp_result == NULL) return OKAY;
    int orig_len = (int)wcslen(wcp_original);
    if (i_start >= orig_len) { wcp_result[0] = L'\0'; return OKAY; }
    int copy_len = i_length;
    if (i_start + copy_len > orig_len) copy_len = orig_len - i_start;
    if (copy_len >= i_result_buf_size) copy_len = i_result_buf_size - 1;
    wcsncpy(wcp_result, wcp_original + i_start, (size_t)copy_len);
    wcp_result[copy_len] = L'\0';
    return OKAY;
}

/*
 * pstrCompare_Wide() — case-insensitive wide string compare
 */
int PT_DECLSPEC pstrCompare_Wide(wchar_t *wcp_str1, wchar_t *wcp_str2, int *ip_equal)
{
    if (ip_equal) *ip_equal = IS_FALSE;
    if (wcp_str1 == NULL || wcp_str2 == NULL) return OKAY;
    if (wcscasecmp(wcp_str1, wcp_str2) == 0)
        if (ip_equal) *ip_equal = IS_TRUE;
    return OKAY;
}

/*
 * pstrCalcGUID_Wide() — generate a GUID-like string (stub)
 */
int PT_DECLSPEC pstrCalcGUID_Wide(wchar_t *wcp_guid, int i_buf_size)
{
    if (wcp_guid == NULL || i_buf_size < 40) return NOT_OKAY;
    // Generate a simple pseudo-GUID using random numbers
    unsigned int a = (unsigned int)rand(), b = (unsigned int)rand();
    unsigned int c = (unsigned int)rand(), d = (unsigned int)rand();
    swprintf(wcp_guid, (size_t)i_buf_size,
             L"{%08X-%04X-%04X-%04X-%08X%04X}",
             a, b & 0xFFFF, c & 0xFFFF, d & 0xFFFF, a ^ b, c ^ d);
    return OKAY;
}

/*
 * pstrGetLastErrorString_Wide() — stub
 */
int PT_DECLSPEC pstrGetLastErrorString_Wide(wchar_t *wcp_error, int i_buffer_length)
{
    if (wcp_error && i_buffer_length > 0) wcp_error[0] = L'\0';
    return OKAY;
}

/*
 * pstrTruncateString_Wide() — truncate to max length
 */
int PT_DECLSPEC pstrTruncateString_Wide(wchar_t *wcp_original, int i_max_length,
                                         wchar_t *wcp_truncated, int i_truncated_buf_size)
{
    if (wcp_original == NULL || wcp_truncated == NULL) return OKAY;
    int len = (int)wcslen(wcp_original);
    int copy_len = (len < i_max_length) ? len : i_max_length;
    if (copy_len >= i_truncated_buf_size) copy_len = i_truncated_buf_size - 1;
    wcsncpy(wcp_truncated, wcp_original, (size_t)copy_len);
    wcp_truncated[copy_len] = L'\0';
    return OKAY;
}

#endif // __APPLE__
