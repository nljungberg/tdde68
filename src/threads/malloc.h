#ifndef THREADS_MALLOC_H
#define THREADS_MALLOC_H

#include <debug.h>
#include <stddef.h>

#define malloc(size) malloc_orig(size, __func__)
#define LEAK_CALLER_STR_SIZE 30
#define LEAK_NUM_TRACKERS 512

void malloc_init (void);
void *malloc_orig (size_t, const char *func) __attribute__ ((malloc));
void *calloc (size_t, size_t) __attribute__ ((malloc));
void *realloc (void *, size_t);
void free (void *);
void check_malloc_exit ();
#endif /* threads/malloc.h */
