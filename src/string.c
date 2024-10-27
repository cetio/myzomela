#include <immintrin.h>
#include <stdint.h>

/// @brief Sets all bytes at ptr to val for the given length.
/// @param ptr Pointer destination.
/// @param val Value to set all bytes to.
/// @param len Number of bytes to be set to val.
/// @return The pointer originally provided.
void* memset(void* restrict ptr, uint8_t val, size_t len)
{
    register void* ret = ptr;
    // NOTE: Checking for alignment is worse for performance on modern hardware, but for
    // the sake of older hardware less optimized for unaligned operations, we must check.
#if __AVX__
    for (int i = 0; i < (len / 32); i++)
    {
        const register __m256i vec = _mm256_set1_epi8(val);
        if ((size_t)ptr % 32)
            _mm256_storeu_si256((__m256i*)ptr, vec);
        else
            _mm256_store_si256((__m256i*)ptr, vec);
        ptr += 32;
    }
    len %= 32;
#elif __SSE2__
    for (int i = 0; i < (len / 16); i++)
    {
        const register __m128i vec = _mm_set1_epi8(val);
        if ((size_t)ptr % 16)
            _mm_storeu_si128((__m128i*)ptr, vec);
        else
            _mm_store_si128((__m128i*)ptr, vec);
        ptr += 16;
    }
    len %= 16;
#endif
    while (--len >= 0)
        *(uint8_t*)ptr++ = val;
    return ret;
}

/// @brief Copies len number bytes from src to dst.
/// @param dst Destination pointer.
/// @param src Source pointer.
/// @param len Number of bytes to be copied.
/// @return The destination pointer originally provided.
void* memcpy(void* restrict dst, void* restrict src, size_t len)
{
    register void* ret = dst;
#if __AVX__
    for (int i = 0; i < (len / 32); i++)
    {
        if ((size_t)dst % 32 || (size_t)src % 32)
            _mm256_storeu_si256((__m256i*)dst, _mm256_loadu_si256((__m256i*)src));
        else
            _mm256_store_si256((__m256i*)dst, _mm256_load_si256((__m256i*)src));
        dst += 32;
        src += 32;
    }
    len %= 32;
#elif __SSE2__
    for (int i = 0; i < (len / 16); i++)
    {
        if ((size_t)dst % 16 || (size_t)src % 16)
            _mm_storeu_si128((__m128i*)dst, _mm_loadu_si128((__m128i*)src));
        else
            _mm_store_si128((__m128i*)dst, _mm_load_si128((__m128i*)src));
        dst += 16;
        src += 16;
    }
    len %= 16;
#endif
    while (--len >= 0)
        *(uint8_t*)dst++ = *(uint8_t*)src++;
    return ret;
}

/// @brief Calculates the length of the string str, searching for the null terminator.
/// @param str Pointer to the string to calculate the length of.
/// @return The length of str.
int strlen(char* str)
{
    int res = 0;
    int mask;
    do
    {
        mask = _mm256_cmpeq_epi8_mask(_mm256_loadu_si256((__m256i*)str), _mm256_setzero_si256());
        // ctz is defined to have undefined behavior sometimes.
        mask = mask == 0 ? 32 : __builtin_ctz(mask);
        res += mask;
        str += 32;
    } while (mask == 32);
    return res;
}

/// @brief Searches the string str for the first instance of character c.
/// @param str Pointer to the string to search.
/// @param c Character to search for.
/// @return The pointer to the first instance of c.
char* strchr(char* str, char c)
{
    int mask;
    do
    {
        mask = _mm256_cmpeq_epi8_mask(_mm256_loadu_si256((__m256i*)str), _mm256_set1_epi8(c));
        mask = mask == 0 ? 32 : __builtin_ctz(mask);
        str += 32;
    } while (mask == 32);
    return str - 32 + mask;
}