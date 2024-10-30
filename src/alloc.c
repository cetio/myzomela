#include <immintrin.h>
#include <stdint.h>
#include <sys/mman.h>
#include "bitmap.h"
#include <stdio.h>
#include <string.h>

#define SLAB_SIZE 1024 * 1024 * 4
#define LARGE_CLUSTERS 16
#define SMALL_CLUSTERS 256

static struct Slab* slab;

struct Slab
{
    __m256i large[LARGE_CLUSTERS];
    __m256i small[SMALL_CLUSTERS];
    void* ptr;
    struct Slab* next;
};

struct Slab* slab_init()
{
    void* ptr = mmap(NULL, SLAB_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ptr += 32 - ((size_t)ptr % 32);
    memset(ptr, 255, ((LARGE_CLUSTERS + SMALL_CLUSTERS) * 32));
    memset(ptr + ((LARGE_CLUSTERS + SMALL_CLUSTERS) * 32), 0, sizeof(struct Slab) - ((LARGE_CLUSTERS + SMALL_CLUSTERS) * 32));
    return (struct Slab*)ptr;
}

void* alloc(size_t size)
{
    if (slab == NULL)
        slab = slab_init();

    struct Slab* cur = slab;
    while (cur != NULL)
    {
        int shard = -1;
        for (int i = 0; i < SMALL_CLUSTERS; i++)
        {
            int res = bitmapSDecode8Successive(cur->small + i, size / 48);
            shard = res == -1 ? shard : res;
        }

        printf("%d", shard);
        cur = cur->next;
    }

    return NULL;
}