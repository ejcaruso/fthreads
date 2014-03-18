#ifndef FT_ASSERT_H
#define FT_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#define ftpanic(msg)                                \
  do {                                              \
    fprintf(stderr, "Panic at %s:%d: " msg "\n",    \
            __FILE__, __LINE__);                    \
    exit(-1);                                       \
  } while(0)

#define ftassert(val) \
  do {                                              \
    if (!(val)) {                                   \
      ftpanic("Assertion \"" #val "\" failed.");    \
    }                                               \
  } while(0)

#endif
