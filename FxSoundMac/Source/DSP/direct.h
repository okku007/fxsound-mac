// macOS stub for <direct.h> — Windows directory functions.
// Provide POSIX equivalents.
#pragma once
#include <sys/stat.h>
#include <unistd.h>

inline int _mkdir(const char* path) { return mkdir(path, 0755); }
inline int _rmdir(const char* path) { return rmdir(path); }
inline char* _getcwd(char* buf, int size) { return getcwd(buf, (size_t)size); }
