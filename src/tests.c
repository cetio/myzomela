#include <stdio.h>
#include <immintrin.h>
#include "bitmap.h"
#include "string.h"

#define JMP_EPI8(A, B, C) goto *(A[__builtin_ctz(_mm256_cmpeq_epi8_mask(B, C))])

int main()
{
    printf("%c", *strchr("abd", 'b'));
    static const void* labels[] = { &&a, &&z };
    JMP_EPI8(labels, _mm256_setr_epi8('a', 'z', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), _mm256_set1_epi8('z'));
a:
    printf("branch a");
    return 0;
z:
    printf("branch z");
    return 0;
}