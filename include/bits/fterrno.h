#ifndef FT_BITS_FTERRNO_H
#define FT_BITS_FTERRNO_H

int *__fthread_errno();

#undef errno
#define errno (*__fthread_errno())

#endif
