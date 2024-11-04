#include <stdio.h>
#include <immintrin.h>
#include "mzalloc.h"

int main()
{
    printf("%zu\n", ((size_t)mzalloc(48 * 2)));
    printf("%zu\n", ((size_t)mzalloc(48)));
    printf("%zu\n", ((size_t)mzalloc(48)));
    return 0;
}