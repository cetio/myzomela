#include <immintrin.h>

#define JMP_EPI8(A, B, C) goto *(A[__builtin_ctz(_mm256_cmpeq_epi8_mask(B, C))])
#define JMP_EPI16(A, B, C) goto *(A[__builtin_ctz(_mm256_cmpeq_epi16_mask(B, C))])
#define JMP_EPI32(A, B, C) goto *(A[__builtin_ctz(_mm256_cmpeq_epi32_mask(B, C))])
#define JMP_EPI64(A, B, C) goto *(A[__builtin_ctz(_mm256_cmpeq_epi64_mask(B, C))])

/// @brief Searches for the first set bit in the given bitmap.
/// @param bitmap The 256-bit bitmap to be searched.
/// @return The index of the first set bit, or -1 if not found.
static inline int bitmapDecode(__m256i bitmap)
{
    int mask = _mm256_cmpneq_epi8_mask(bitmap, _mm256_setzero_si256());
    if (mask == 0)
        return -1;
    else if (mask & 0b11110000)
        bitmap = _mm256_permute4x64_epi64(bitmap, 0b01001110);
    
    mask = __builtin_ctz(mask);
    bitmap = _mm256_shuffle_epi8(bitmap, _mm256_set1_epi8(mask % 16));
    return __builtin_ctz(_mm256_extract_epi8(bitmap, 0)) + (mask << 3);
}

/// @brief Searches for the first successive set bit pattern of num in the given bitmap.
/// @param bitmap The 256-bit bitmap to be searched.
/// @param num The number of successive bits to match for.
/// @return The index of the first set bit in the pattern, or -1 if not found.
static inline int bitmapDecodeSuccessive(__m256i bitmap, int num)
{
    while (--num > 0)
        bitmap = _mm256_and_si256(bitmap, _mm256_ror_epi64(bitmap, 1));
    return bitmapDecode(bitmap);
}

/// @brief Searches for the first successive bit pattern of num in the given 8 bitmaps at ptr, and unsets the bit pattern found.
/// @param ptr Pointer to 8 256-bit bitmaps to be searched. Must be aligned to 32-bytes.
/// @param num The number of successive bits to match for.
/// @return The index of the first set bit in the pattern, or -1 if not found.
static inline int bitmapSDecode8Successive(__m256i* ptr, int num)
{
    __m256i bitmap;
    __m256i bitmaps[8] = {
        _mm256_load_si256(ptr),
        _mm256_load_si256(ptr + 1),
        _mm256_load_si256(ptr + 2),
        _mm256_load_si256(ptr + 3),
        _mm256_load_si256(ptr + 4),
        _mm256_load_si256(ptr + 5),
        _mm256_load_si256(ptr + 6),
        _mm256_load_si256(ptr + 7),
    };
    
    int mask = 0;
    int index;
    for (int i = 7; i >= 0; i--)
    {
        int res = _mm256_cmpneq_epi8_mask(bitmaps[i], _mm256_setzero_si256());
        mask = res == 0 ? mask : res;
        index = res == 0 ? index : i;
    }

    if (mask == 0)
        return -1;

    bitmap = bitmaps[index];
    _mm256_store_si256(ptr + index, _mm256_setzero_si256());
    if (mask & 0b11110000)
        bitmap = _mm256_permute4x64_epi64(bitmap, 0b01001110);

    while (--num > 0)
        bitmap = _mm256_and_si256(bitmap, _mm256_ror_epi64(bitmap, 1));

    mask = __builtin_ctz(mask);
    bitmap = _mm256_shuffle_epi8(bitmap, _mm256_set1_epi8(mask % 16));
    return _mm256_extract_epi8(bitmap, 0) == 0 
        ? -1 
        : __builtin_ctz(_mm256_extract_epi8(bitmap, 0)) + (mask << 3) + (index * 256);
}