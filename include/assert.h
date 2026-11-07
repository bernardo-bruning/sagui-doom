#ifndef DOOM_ASSERT_H
#define DOOM_ASSERT_H
#include <stdlib.h>
#define assert(e) ((e) ? (void)0 : abort())
#endif
