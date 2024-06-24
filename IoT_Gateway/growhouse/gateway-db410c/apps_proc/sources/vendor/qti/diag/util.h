#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

void print_hex_dump(const char *prefix, const void *buf, size_t len);

#define container_of(ptr, type, member) ({ \
		const typeof(((type *)0)->member)*__mptr = (ptr);  \
		(type *)((char *)__mptr - offsetof(type, member)); \
		})

#define QUAD_ALIGNED(x) ((x % 4) > 0 ? x + (4 - x % 4) : x)

#define ALLOCATION_MAP_SIZE 500
extern void *diag_malloc(uint8_t align, size_t s);
extern size_t diag_free(void *p);
extern void diag_alloc_map();
#endif
