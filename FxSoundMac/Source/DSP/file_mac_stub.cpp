// macOS stub implementations for file utility functions needed by the DSP layer.
// Provides POSIX-based implementations of the Windows-oriented file functions.
#if defined(__APPLE__)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "codedefs.h"
#include "slout.h"
#include "file.h"

// Helper: convert wide string to narrow for POSIX calls
static void wcs_to_mbs(const wchar_t* wcs, char* mbs, size_t mbs_size)
{
    wcstombs(mbs, wcs, mbs_size);
    mbs[mbs_size - 1] = '\0';
}

FILE PT_DECLSPEC *fileOpen(char *cp_name, char *cp_mode, CSlout *)
{
    return fopen(cp_name, cp_mode);
}

FILE PT_DECLSPEC *fileOpen_Wide(wchar_t *wcp_name, wchar_t *wcp_mode, CSlout *)
{
    char name[4096], mode[64];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    wcs_to_mbs(wcp_mode, mode, sizeof(mode));
    return fopen(name, mode);
}

int PT_DECLSPEC fileExist(char *cp_name, int *ip_exist)
{
    if (ip_exist) *ip_exist = 0;
    if (cp_name == NULL) return OKAY;
    struct stat st;
    if (stat(cp_name, &st) == 0) { if (ip_exist) *ip_exist = 1; }
    return OKAY;
}

int PT_DECLSPEC fileExist_Wide(wchar_t *wcp_name, int *ip_exist)
{
    if (ip_exist) *ip_exist = 0;
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    struct stat st;
    if (stat(name, &st) == 0) { if (ip_exist) *ip_exist = 1; }
    return OKAY;
}

int PT_DECLSPEC fileDoesPathExist_Wide(wchar_t *wcp_file_path, int *ip_exist)
{
    return fileExist_Wide(wcp_file_path, ip_exist);
}

int PT_DECLSPEC fileRemove(char *cp_name, CSlout *)
{
    if (cp_name == NULL) return OKAY;
    remove(cp_name);
    return OKAY;
}

int PT_DECLSPEC fileRemove_Wide(wchar_t *wcp_name, CSlout *)
{
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    remove(name);
    return OKAY;
}

int PT_DECLSPEC fileRemoveWithStatus_Wide(wchar_t *wcp_name, int *ip_removed, CSlout *)
{
    if (ip_removed) *ip_removed = 0;
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    if (remove(name) == 0) { if (ip_removed) *ip_removed = 1; }
    return OKAY;
}

int PT_DECLSPEC fileSize_Wide(wchar_t *wcp_name, uint64_t *ip_size, CSlout *)
{
    if (ip_size) *ip_size = 0;
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    struct stat st;
    if (stat(name, &st) == 0) { if (ip_size) *ip_size = (uint64_t)st.st_size; }
    return OKAY;
}

int PT_DECLSPEC fileToString(char *cp_name, char *cp_buf, int i_buf_size, CSlout *)
{
    if (cp_name == NULL || cp_buf == NULL) return OKAY;
    FILE *f = fopen(cp_name, "r");
    if (!f) return OKAY;
    size_t n = fread(cp_buf, 1, (size_t)(i_buf_size - 1), f);
    cp_buf[n] = '\0';
    fclose(f);
    return OKAY;
}

int PT_DECLSPEC fileToString_Wide(wchar_t *wcp_name, char *cp_buf, int i_buf_size, CSlout *)
{
    if (wcp_name == NULL || cp_buf == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    return fileToString(name, cp_buf, i_buf_size, NULL);
}

int PT_DECLSPEC fileToString_WithAllocation_Wide(wchar_t *wcp_name, wchar_t **wcpp_buf, int *ip_len, CSlout *)
{
    if (wcpp_buf) *wcpp_buf = NULL;
    if (ip_len) *ip_len = 0;
    return OKAY;
}

int PT_DECLSPEC fileWriteString_Wide(wchar_t *wcp_name, wchar_t *wcp_str, CSlout *)
{
    if (wcp_name == NULL || wcp_str == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    FILE *f = fopen(name, "w");
    if (!f) return OKAY;
    char buf[65536];
    wcs_to_mbs(wcp_str, buf, sizeof(buf));
    fputs(buf, f);
    fclose(f);
    return OKAY;
}

int PT_DECLSPEC fileIsNormalFile(char *cp_name, int *ip_is_file, CSlout *)
{
    if (ip_is_file) *ip_is_file = 0;
    if (cp_name == NULL) return OKAY;
    struct stat st;
    if (stat(cp_name, &st) == 0 && S_ISREG(st.st_mode)) { if (ip_is_file) *ip_is_file = 1; }
    return OKAY;
}

int PT_DECLSPEC fileIsNormalFile_Wide(wchar_t *wcp_name, int *ip_is_file, CSlout *)
{
    if (ip_is_file) *ip_is_file = 0;
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    return fileIsNormalFile(name, ip_is_file, NULL);
}

int PT_DECLSPEC fileIsDirectory(char *cp_name, int *ip_is_dir, CSlout *)
{
    if (ip_is_dir) *ip_is_dir = 0;
    if (cp_name == NULL) return OKAY;
    struct stat st;
    if (stat(cp_name, &st) == 0 && S_ISDIR(st.st_mode)) { if (ip_is_dir) *ip_is_dir = 1; }
    return OKAY;
}

int PT_DECLSPEC fileIsDirectory_Wide(wchar_t *wcp_name, int *ip_is_dir, CSlout *)
{
    if (ip_is_dir) *ip_is_dir = 0;
    if (wcp_name == NULL) return OKAY;
    char name[4096];
    wcs_to_mbs(wcp_name, name, sizeof(name));
    return fileIsDirectory(name, ip_is_dir, NULL);
}

int PT_DECLSPEC fileSplitFullpath(char *cp_fullpath, char *cp_dir, char *cp_file, CSlout *)
{
    if (cp_fullpath == NULL) return OKAY;
    const char *last_slash = strrchr(cp_fullpath, '/');
    if (!last_slash) last_slash = strrchr(cp_fullpath, '\\');
    if (last_slash) {
        if (cp_dir) { strncpy(cp_dir, cp_fullpath, last_slash - cp_fullpath); cp_dir[last_slash - cp_fullpath] = '\0'; }
        if (cp_file) strcpy(cp_file, last_slash + 1);
    } else {
        if (cp_dir) cp_dir[0] = '\0';
        if (cp_file) strcpy(cp_file, cp_fullpath);
    }
    return OKAY;
}

int PT_DECLSPEC fileSplitFullpath_Wide(wchar_t *wcp_fullpath, wchar_t *wcp_dir, wchar_t *wcp_file, CSlout *)
{
    if (wcp_fullpath == NULL) return OKAY;
    const wchar_t *last_slash = wcsrchr(wcp_fullpath, L'/');
    const wchar_t *last_bslash = wcsrchr(wcp_fullpath, L'\\');
    const wchar_t *split = (last_slash > last_bslash) ? last_slash : last_bslash;
    if (split) {
        if (wcp_dir) { wcsncpy(wcp_dir, wcp_fullpath, split - wcp_fullpath); wcp_dir[split - wcp_fullpath] = L'\0'; }
        if (wcp_file) wcscpy(wcp_file, split + 1);
    } else {
        if (wcp_dir) wcp_dir[0] = L'\0';
        if (wcp_file) wcscpy(wcp_file, wcp_fullpath);
    }
    return OKAY;
}

int PT_DECLSPEC fileGetDriveFromFullpath_Wide(wchar_t *, wchar_t *wcp_drive, CSlout *)
{
    if (wcp_drive) wcp_drive[0] = L'\0';
    return OKAY;
}

int PT_DECLSPEC fileNumLines(char *, int *ip_lines, CSlout *) { if (ip_lines) *ip_lines = 0; return OKAY; }
int PT_DECLSPEC fileNumLines_Wide(wchar_t *, int *ip_lines, CSlout *) { if (ip_lines) *ip_lines = 0; return OKAY; }
int PT_DECLSPEC fileOkToWriteInDir(char *, int *ip_ok, CSlout *) { if (ip_ok) *ip_ok = 1; return OKAY; }
int PT_DECLSPEC fileOkToWriteInDir_Wide(wchar_t *, int *ip_ok, CSlout *) { if (ip_ok) *ip_ok = 1; return OKAY; }
int PT_DECLSPEC fileReadNextNonBlankLine_Wide(FILE *, wchar_t *, int, wchar_t *, int, int, int *ip_eof) { if (ip_eof) *ip_eof = 1; return OKAY; }

int PT_DECLSPEC fileCreateDirectory(char *cp_path, int *ip_created, CSlout *)
{
    if (ip_created) *ip_created = 0;
    if (cp_path == NULL) return OKAY;
    if (mkdir(cp_path, 0755) == 0) { if (ip_created) *ip_created = 1; }
    return OKAY;
}

int PT_DECLSPEC fileCreateDirectory_Wide(wchar_t *wcp_path, int *ip_created, CSlout *)
{
    if (ip_created) *ip_created = 0;
    if (wcp_path == NULL) return OKAY;
    char path[4096];
    wcs_to_mbs(wcp_path, path, sizeof(path));
    return fileCreateDirectory(path, ip_created, NULL);
}

int PT_DECLSPEC fileWaitOnWriteBlock_Wide(wchar_t *, int *ip_blocked, CSlout *) { if (ip_blocked) *ip_blocked = 0; return OKAY; }
int PT_DECLSPEC fileGetUNCFolderFromUNCpath_Wide(wchar_t *, wchar_t *wcp_folder, CSlout *) { if (wcp_folder) wcp_folder[0] = L'\0'; return OKAY; }

// FileDsp.cpp
int PT_DECLSPEC fileCalcDspPath(char *, char *, char *, char *, char *, int, int, int, int, int, float, float) { return OKAY; }

// FileDate.cpp stubs
int PT_DECLSPEC fileSetBackCreateTime(char *, long, CSlout *) { return OKAY; }
int PT_DECLSPEC fileSetBackCreateTime_Wide(wchar_t *, long, CSlout *) { return OKAY; }
int PT_DECLSPEC fileCompareDates(char *, char *, int *ip_result, CSlout *) { if (ip_result) *ip_result = 0; return OKAY; }
int PT_DECLSPEC fileCompareDates_Wide(wchar_t *, wchar_t *, int *ip_result, CSlout *) { if (ip_result) *ip_result = 0; return OKAY; }
int PT_DECLSPEC fileTouch(char *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileTouch_Wide(wchar_t *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileGetModifiedDateString(char *, char *cp_date, CSlout *) { if (cp_date) cp_date[0] = '\0'; return OKAY; }
int PT_DECLSPEC fileGetModifiedDateString_Wide(wchar_t *, wchar_t *wcp_date, CSlout *) { if (wcp_date) wcp_date[0] = L'\0'; return OKAY; }

// FileRegistry.cpp stubs
int PT_DECLSPEC fileRegCreateKey_Wide(wchar_t *, wchar_t *, wchar_t *) { return OKAY; }
int PT_DECLSPEC fileRegReadKey_Wide(wchar_t *, int *ip_exists, wchar_t *wcp_val, unsigned long) { if (ip_exists) *ip_exists = 0; if (wcp_val) wcp_val[0] = L'\0'; return OKAY; }
int PT_DECLSPEC fileRegRemoveKey_Wide(wchar_t *, wchar_t *) { return OKAY; }

// FileTrace.cpp stub
int PT_DECLSPEC fileTrace_Wide(wchar_t *, wchar_t *, CSlout *) { return OKAY; }

// FileRm.cpp stubs
int PT_DECLSPEC fileDirRemove(char *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileDirRemove_Wide(wchar_t *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileRemoveWithWildcard_Wide(wchar_t *, CSlout *) { return OKAY; }

// FileTail.cpp stub
int PT_DECLSPEC fileTail_Wide(wchar_t *, wchar_t *, int, CSlout *) { return OKAY; }

// FileAray.cpp stubs
int PT_DECLSPEC fileCreateArrayOfStrings_Wide(wchar_t *, wchar_t ***, int *ip_count, int, int, int, CSlout *) { if (ip_count) *ip_count = 0; return OKAY; }
int PT_DECLSPEC fileWriteArrayOfStrings_Wide(wchar_t *, wchar_t **, int, int, CSlout *) { return OKAY; }
int PT_DECLSPEC fileFreeArrayOfStrings_Wide(wchar_t **wcpp, int n, CSlout *) {
    if (wcpp) { for (int i = 0; i < n; i++) if (wcpp[i]) free(wcpp[i]); free(wcpp); }
    return OKAY;
}

// FileCopy.cpp stubs
int PT_DECLSPEC fileCopy(char *, char *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileCopy_Wide(wchar_t *, wchar_t *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileCopyListOfFullpaths_Wide(wchar_t **, int, wchar_t *, wchar_t *, wchar_t *, CSlout *) { return OKAY; }
int PT_DECLSPEC fileCreateDirectoryAndParents(char *cp_path, int *ip_created, CSlout *)
{
    if (ip_created) *ip_created = 0;
    if (cp_path == NULL) return OKAY;
    // Simple mkdir -p equivalent
    char tmp[4096];
    strncpy(tmp, cp_path, sizeof(tmp) - 1);
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/' || *p == '\\') {
            *p = '\0';
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) == 0) { if (ip_created) *ip_created = 1; }
    else if (errno == EEXIST) { if (ip_created) *ip_created = 1; }
    return OKAY;
}
int PT_DECLSPEC fileCreateDirectoryAndParents_Wide(wchar_t *wcp_path, int *ip_created, CSlout *)
{
    if (wcp_path == NULL) return OKAY;
    char path[4096];
    wcs_to_mbs(wcp_path, path, sizeof(path));
    return fileCreateDirectoryAndParents(path, ip_created, NULL);
}
int PT_DECLSPEC fileCopyFileAndParents(char *, char *, int *ip_copied, CSlout *) { if (ip_copied) *ip_copied = 0; return OKAY; }
int PT_DECLSPEC fileCopyFileAndParents_Wide(wchar_t *, wchar_t *, int *ip_copied, CSlout *) { if (ip_copied) *ip_copied = 0; return OKAY; }
int PT_DECLSPEC fileCopyFolderRecursively_Wide(wchar_t *, wchar_t *, wchar_t *, CSlout *) { return OKAY; }

// FileRecursiveList.cpp stub
int PT_DECLSPEC fileCreateRecursiveListOfAllFullpaths_Wide(wchar_t *, wchar_t ***, int *ip_count, int, wchar_t **, int, wchar_t *, CSlout *) { if (ip_count) *ip_count = 0; return OKAY; }

// FileSubfoldersList.cpp stub
int fileCreateListOfSubfolders_Wide(wchar_t *, wchar_t ***, int *ip_count, int, CSlout *) { if (ip_count) *ip_count = 0; return OKAY; }

// FileSubfilesList.cpp stub
int fileCreateListOfSubfiles_Wide(wchar_t *, wchar_t ***, int *ip_count, int, CSlout *) { if (ip_count) *ip_count = 0; return OKAY; }

// fileSmartStringSearch.cpp stubs
int PT_DECLSPEC fileSmartStringSearchLineBased(wchar_t *, wchar_t *, wchar_t *, char *, wchar_t *, bool, bool *ip_found, bool *) { if (ip_found) *ip_found = false; return OKAY; }
int PT_DECLSPEC fileSmartStringSearchLineBasedSingleLine(wchar_t *, wchar_t *, wchar_t *, char *, wchar_t *, bool, bool *ip_found, bool *) { if (ip_found) *ip_found = false; return OKAY; }

#endif // __APPLE__
