#ifndef DOOM_STDLIB_H
#define DOOM_STDLIB_H
#include <stddef.h>
void *malloc(size_t n);
void *calloc(size_t n, size_t size);
void *realloc(void *p, size_t n);
void free(void *p);
int atoi(const char *s);
long atol(const char *s);
long strtol(const char *s, char **end, int base);
int abs(int n);
double atof(const char *s);
void qsort(void *base, size_t n, size_t size, int (*cmp)(const void *, const void *));
void exit(int code) __attribute__((noreturn));
void abort(void) __attribute__((noreturn));
int atexit(void (*fn)(void));
int system(const char *command);
#endif
