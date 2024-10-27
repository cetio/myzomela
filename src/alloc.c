#include <immintrin.h>
#include <stdint.h>
#include <sys/mman.h>
#include "bitmap.h"

#define SLAB_SIZE 1024 * 1024 * 4
#define LARGE_CLUSTERS 16
#define SMALL_CLUSTERS 256

//static struct Slab* slab = mmap(NULL, SLAB_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

struct Slab
{
    __m256i large[LARGE_CLUSTERS];
    __m256i small[SMALL_CLUSTERS];
    void* ptr;
    struct Slab* next;
};

void* malloc(size_t size)
{
    int shard = 0;
    for (int i = 0; i < SMALL_CLUSTERS; i++)
    {
        shard = bitmapDecodeSuccessive(slab->large[i], size / 48);
        if (shard != -1)
            break;
    }
}