#pragma once
// macOS compatibility shim for the Windows-oriented DfxDsp sources.
// Mirrors the existing __ANDROID__ non-Win32 path in codedefs.h/pt_defs.h.
// This header is included by codedefs.h when __APPLE__ is defined.
// Must compile in both C and C++ modes.
#if defined(__APPLE__)

#ifdef __cplusplus
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#else
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#endif

// Win32 scalar types used by the DSP sources.
typedef int32_t  LONG;
typedef uint32_t DWORD;
typedef int      BOOL;
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef void*    HANDLE;
typedef wchar_t  WCHAR;
typedef unsigned int UINT;
typedef void*    HINSTANCE;
typedef void*    HMODULE;
typedef const wchar_t* LPCWSTR;
typedef const char*    LPCSTR;
typedef wchar_t*       LPWSTR;
typedef char*          LPSTR;
typedef const void*    LPCVOID;
typedef void*          LPVOID;
typedef void*          LPSECURITY_ATTRIBUTES;
typedef const wchar_t* LPCTSTR;
typedef uintptr_t      WPARAM;
typedef intptr_t       LPARAM;
typedef intptr_t       LRESULT;

// FILETIME stub
struct FILETIME { uint32_t dwLowDateTime; uint32_t dwHighDateTime; };
typedef struct FILETIME* LPFILETIME;

// SYSTEMTIME stub
struct SYSTEMTIME { uint16_t wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds; };
typedef struct SYSTEMTIME* LPSYSTEMTIME;

// REG_ constants
#define REG_CURRENT_USER  4
#define REG_LOCAL_MACHINE 2
#define REG_CLASSES_ROOT  1
#define REG_USERS         3

#ifndef HWND
#define HWND void*
#endif

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef MB_OK
#define MB_OK 0
#endif

// Win32 debug helpers
#define IsDebuggerPresent() (0)
#define DebugBreak()        ((void)0)

// Win32 MessageBox stubs
#define MessageBoxW(hwnd, text, cap, type) (0)
#define MessageBoxA(hwnd, text, cap, type) (0)
#define MessageBox(hwnd, text, cap, type)  (0)

// Win32 library loading stubs
#define FreeLibrary(h)          (1)
#define LoadLibraryW(name)      ((void*)0)
#define GetProcAddress(h, name) ((void*)0)

// Windows error stubs
#define FORMAT_MESSAGE_FROM_SYSTEM 0x1000
#define GetLastError()             (0UL)
#define FormatMessage(f,s,e,l,b,n,a) (0)

// CompareFileTime
static inline int CompareFileTime(const struct FILETIME* ft1, const struct FILETIME* ft2) {
    if (!ft1 || !ft2) return 0;
    if (ft1->dwHighDateTime != ft2->dwHighDateTime)
        return (ft1->dwHighDateTime < ft2->dwHighDateTime) ? -1 : 1;
    if (ft1->dwLowDateTime != ft2->dwLowDateTime)
        return (ft1->dwLowDateTime < ft2->dwLowDateTime) ? -1 : 1;
    return 0;
}

// MSVC non-conforming swprintf compat.
// On macOS, swprintf requires a size argument (POSIX standard).
// We provide overloaded wrappers in C++ mode; in C mode use a macro.
#ifdef __cplusplus

// Rename system swprintf to avoid infinite recursion
#define swprintf pt_mac_swprintf_compat

inline int pt_mac_swprintf_compat(wchar_t* buf, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = vswprintf(buf, 4096, fmt, args);
    va_end(args);
    return r;
}

inline int pt_mac_swprintf_compat(wchar_t* buf, size_t n, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int r = vswprintf(buf, n, fmt, args);
    va_end(args);
    return r;
}

// MSVC numeric conversion functions
inline int   _wtoi(const wchar_t* s) { return (int)wcstol(s, nullptr, 10); }
inline long  _wtol(const wchar_t* s) { return wcstol(s, nullptr, 10); }
inline float _wtof(const wchar_t* s) { return (float)wcstod(s, nullptr); }

// MSVC wide string functions
inline int _wcsicmp(const wchar_t* s1, const wchar_t* s2) { return wcscasecmp(s1, s2); }
inline int _wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t n) { return wcsncasecmp(s1, s2, n); }
inline wchar_t* wcstok_s(wchar_t* str, const wchar_t* delim, wchar_t** ctx) { return wcstok(str, delim, ctx); }

// Windows COM/GUID stubs
typedef struct { unsigned long Data1; unsigned short Data2, Data3; unsigned char Data4[8]; } GUID;
#define S_OK 0
inline int CoCreateGuid(GUID* g) { if (g) memset(g, 0, sizeof(GUID)); return S_OK; }

// wsprintf — Windows wide sprintf
#define wsprintf swprintf

// MSVC _wfopen_s
inline int _wfopen_s(FILE** pFile, const wchar_t* filename, const wchar_t* mode)
{
    if (!pFile) return 22;
    char narrow_filename[4096];
    char narrow_mode[64];
    wcstombs(narrow_filename, filename, sizeof(narrow_filename));
    wcstombs(narrow_mode, mode, sizeof(narrow_mode));
    *pFile = fopen(narrow_filename, narrow_mode);
    return (*pFile == nullptr) ? 1 : 0;
}

#else // C mode

// In C mode, use a simple macro for swprintf (2-arg form only)
#define swprintf(buf, fmt, ...) vswprintf(buf, 4096, fmt, (va_list){__VA_ARGS__})

// Numeric conversion macros for C
#define _wtoi(s) ((int)wcstol((s), NULL, 10))
#define _wtol(s) (wcstol((s), NULL, 10))
#define _wtof(s) ((float)wcstod((s), NULL))

// Wide string comparison macros for C
#define _wcsicmp(s1, s2)    wcscasecmp((s1), (s2))
#define _wcsnicmp(s1, s2, n) wcsncasecmp((s1), (s2), (n))
#define wcstok_s(s, d, ctx) wcstok((s), (d), (ctx))

#define wsprintf swprintf

#endif // __cplusplus

#endif // __APPLE__
