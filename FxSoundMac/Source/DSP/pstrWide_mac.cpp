// macOS implementation of pstrWide functions.
// Replaces the UTF-16 encoded pstrWide.cpp which cannot be compiled by clang.
// Uses POSIX wcstombs/mbstowcs instead of Windows MultiByteToWideChar/WideCharToMultiByte.
#if defined(__APPLE__)

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "codedefs.h"
#include "slout.h"
#include "pstr.h"

int PT_DECLSPEC pstrConvertToWideCharString(char *cp_original_string,
                                             wchar_t *wcp_converted_string,
                                             int i_wide_char_buffer_size)
{
    if (cp_original_string == NULL) return NOT_OKAY;
    if (wcp_converted_string == NULL) return NOT_OKAY;
    size_t result = mbstowcs(wcp_converted_string, cp_original_string, (size_t)i_wide_char_buffer_size);
    if (result == (size_t)-1) return NOT_OKAY;
    return OKAY;
}

int PT_DECLSPEC pstrConvertToWideCharString_WithAlloc(char *cp_original_string,
                                                       wchar_t **wcpp_converted_string,
                                                       int *ip_wide_char_buffer_size)
{
    if (cp_original_string == NULL) return NOT_OKAY;
    if (wcpp_converted_string == NULL) return NOT_OKAY;
    size_t len = strlen(cp_original_string) + 1;
    *ip_wide_char_buffer_size = (int)len;
    *wcpp_converted_string = (wchar_t *)calloc(len, sizeof(wchar_t));
    if (*wcpp_converted_string == NULL) return NOT_OKAY;
    if (pstrConvertToWideCharString(cp_original_string, *wcpp_converted_string, (int)len) != OKAY) {
        free(*wcpp_converted_string);
        *wcpp_converted_string = NULL;
        return NOT_OKAY;
    }
    return OKAY;
}

int PT_DECLSPEC pstrConvertWideCharStringToAnsiCharString(wchar_t *wcp_original_string,
                                                           char *cp_converted_string,
                                                           int i_ansi_char_buffer_size)
{
    if (wcp_original_string == NULL) return NOT_OKAY;
    if (cp_converted_string == NULL) return NOT_OKAY;
    size_t result = wcstombs(cp_converted_string, wcp_original_string, (size_t)i_ansi_char_buffer_size);
    if (result == (size_t)-1) return NOT_OKAY;
    return OKAY;
}

int PT_DECLSPEC pstrCovertWideCharStringToUTF8String_WithAlloc(wchar_t *wcp_original_string,
                                                                 char **cpp_converted_utf8_string,
                                                                 int *ip_number_of_bytes)
{
    *cpp_converted_utf8_string = NULL;
    *ip_number_of_bytes = 0;
    if (wcp_original_string == NULL) return NOT_OKAY;

    // Calculate required buffer size
    size_t len = wcslen(wcp_original_string);
    size_t buf_size = len * 4 + 1; // UTF-8 can be up to 4 bytes per wchar_t
    *cpp_converted_utf8_string = (char *)calloc(buf_size, sizeof(char));
    if (*cpp_converted_utf8_string == NULL) return NOT_OKAY;

    size_t result = wcstombs(*cpp_converted_utf8_string, wcp_original_string, buf_size);
    if (result == (size_t)-1) {
        free(*cpp_converted_utf8_string);
        *cpp_converted_utf8_string = NULL;
        return NOT_OKAY;
    }
    *ip_number_of_bytes = (int)(result + 1); // include null terminator
    return OKAY;
}

int PT_DECLSPEC pstrCovertUTF8StringToWideCharString_WithAlloc(char *cp_original_utf8_string,
                                                                 wchar_t **wcpp_converted_string,
                                                                 int *ip_number_of_wchar)
{
    *wcpp_converted_string = NULL;
    *ip_number_of_wchar = 0;
    if (cp_original_utf8_string == NULL) return NOT_OKAY;

    size_t len = strlen(cp_original_utf8_string) + 1;
    *wcpp_converted_string = (wchar_t *)calloc(len, sizeof(wchar_t));
    if (*wcpp_converted_string == NULL) return NOT_OKAY;

    size_t result = mbstowcs(*wcpp_converted_string, cp_original_utf8_string, len);
    if (result == (size_t)-1) {
        free(*wcpp_converted_string);
        *wcpp_converted_string = NULL;
        return NOT_OKAY;
    }
    *ip_number_of_wchar = (int)(result + 1);
    return OKAY;
}

#endif // __APPLE__
