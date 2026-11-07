#ifndef DOOM_UNISTD_H
#define DOOM_UNISTD_H
#include <stddef.h>
int read(int fd, void *buf, unsigned n);
int write(int fd, const void *buf, unsigned n);
int close(int fd);
int unlink(const char *path);
int rename(const char *a, const char *b);
int access(const char *path, int mode);
int isatty(int fd);
int usleep(unsigned usec);
int mkdir(const char *path, unsigned mode);
int rmdir(const char *path);
#endif
