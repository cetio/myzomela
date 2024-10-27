#include <stdint.h>
#include <stddef.h>

/// @brief Sets all bytes at ptr to val for the given length.
/// @param ptr Pointer destination.
/// @param val Value to set all bytes to.
/// @param len Number of bytes to be set to val.
/// @return The pointer originally provided.
void* memset(void* ptr, uint8_t val, size_t len);

/// @brief Copies len number bytes from src to dst.
/// @param dst Destination pointer.
/// @param src Source pointer.
/// @param len Number of bytes to be copied.
/// @return The destination pointer originally provided.
void* memcpy(void* dst, void* src, size_t len);

/// @brief Calculates the length of the string str, searching for the null terminator.
/// @param str Pointer to the string to calculate the length of.
/// @return The length of str.
int strlen(char* str);

/// @brief Searches the string str for the first instance of character c.
/// @param str Pointer to the string to search.
/// @param c Character to search for.
/// @return The pointer to the first instance of c.
char* strchr(char* str, char c);