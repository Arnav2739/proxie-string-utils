/*
 * lib.cpp — C++ Implementation of String Utilities
 *
 * DESIGN DECISIONS:
 * ─────────────────
 * 1. We use malloc/free (not new/delete) because:
 *    - malloc/free are C-standard and work reliably across FFI boundaries
 *    - new/delete are C++ specific and may invoke constructors/destructors
 *    - Using the same allocator on both sides prevents heap corruption
 *
 * 2. Every function checks for NULL input (defensive programming)
 *
 * 3. String-returning functions allocate NEW memory — the caller owns it
 *    and MUST free it via free_string()
 */

#include "wrapper.h"
#include <cstring>   // strlen, strcpy
#include <cstdlib>   // malloc, free
#include <cctype>    // toupper

/*
 * str_length — Calculate string length
 *
 * How it works:
 *   - Guards against NULL pointer
 *   - Uses standard strlen() which counts characters until '\0'
 *
 * Time:  O(n) where n = string length
 * Space: O(1) — no allocation needed
 */
int str_length(const char* s) {
    if (s == nullptr) {
        return 0;
    }
    return static_cast<int>(strlen(s));
}

/*
 * str_reverse — Return a new reversed copy of the string
 *
 * How it works:
 *   1. Get the length of the input string
 *   2. Allocate a new buffer of (length + 1) bytes (+1 for null terminator)
 *   3. Copy characters in reverse order
 *   4. Add null terminator at the end
 *
 * Memory: Caller MUST free the returned pointer via free_string()
 * Time:   O(n)
 * Space:  O(n) — allocates a new string
 */
char* str_reverse(const char* s) {
    if (s == nullptr) {
        return nullptr;
    }

    size_t len = strlen(s);

    // Allocate memory for reversed string + null terminator
    char* reversed = static_cast<char*>(malloc(len + 1));
    if (reversed == nullptr) {
        return nullptr;  // malloc failed — out of memory
    }

    // Copy characters in reverse order
    for (size_t i = 0; i < len; i++) {
        reversed[i] = s[len - 1 - i];
    }

    // Null-terminate the string
    reversed[len] = '\0';

    return reversed;
}

/*
 * count_vowels — Count vowels (a, e, i, o, u) in a string
 *
 * How it works:
 *   - Iterates through each character
 *   - Checks against both lowercase and uppercase vowels
 *   - Uses a switch statement for clarity and performance
 *
 * Time:  O(n)
 * Space: O(1)
 */
int count_vowels(const char* s) {
    if (s == nullptr) {
        return 0;
    }

    int count = 0;

    for (size_t i = 0; s[i] != '\0'; i++) {
        switch (s[i]) {
            case 'a': case 'e': case 'i': case 'o': case 'u':
            case 'A': case 'E': case 'I': case 'O': case 'U':
                count++;
                break;
            default:
                break;
        }
    }

    return count;
}

/*
 * to_uppercase — Return a new uppercase copy of the string
 *
 * How it works:
 *   1. Allocate a new buffer of (length + 1) bytes
 *   2. Convert each character using toupper()
 *      - toupper() only changes lowercase letters (a-z → A-Z)
 *      - All other characters (digits, symbols, etc.) pass through unchanged
 *   3. Add null terminator
 *
 * Memory: Caller MUST free the returned pointer via free_string()
 * Time:   O(n)
 * Space:  O(n) — allocates a new string
 */
char* to_uppercase(const char* s) {
    if (s == nullptr) {
        return nullptr;
    }

    size_t len = strlen(s);

    // Allocate memory for uppercase string + null terminator
    char* upper = static_cast<char*>(malloc(len + 1));
    if (upper == nullptr) {
        return nullptr;  // malloc failed — out of memory
    }

    // Convert each character to uppercase
    for (size_t i = 0; i < len; i++) {
        upper[i] = static_cast<char>(toupper(static_cast<unsigned char>(s[i])));
    }

    // Null-terminate the string
    upper[len] = '\0';

    return upper;
}

/*
 * free_string — Free memory allocated by str_reverse() or to_uppercase()
 *
 * Why this exists:
 *   The "golden rule" of FFI memory management:
 *   → Whoever ALLOCATES the memory must FREE it.
 *
 *   Since C++ allocated the memory (via malloc), C++ must free it (via free).
 *   Rust calling its own deallocator on C++-allocated memory could cause
 *   heap corruption if different allocators are used.
 *
 * Safe to call with NULL — free(NULL) is defined as a no-op in C standard.
 */
void free_string(char* s) {
    free(s);
}
