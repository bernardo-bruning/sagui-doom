#ifndef DOOM_SYS_STAT_H
#define DOOM_SYS_STAT_H
#include <sys/types.h>
struct stat { unsigned st_mode, st_size; };
#define S_IFREG 0100000
#define S_IFDIR 0040000
#define S_ISREG(m) (((m) & S_IFREG) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFDIR) == S_IFDIR)
int stat(const char *path, struct stat *out);
int mkdir(const char *path, unsigned mode);
#endif
