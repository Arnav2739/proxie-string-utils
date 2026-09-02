/*
 * wrapper.h — C-Compatible Header for String Utilities
 *
 * WHY extern "C"?
 * ───────────────
 * C++ compilers "mangle" function names (e.g., str_length becomes _Z10str_lengthPKc)
 * to support overloading. extern "C" tells the compiler: "Use plain C-style names"
 * so that Rust/bindgen can find these functions by their exact name.
 *
 * WHY const char* for inputs?
 * ──────────────────────────
 * This is the standard C string type — a pointer to a null-terminated array of chars.
 * Both C++ and Rust (via CString) understand this format.
 *
 * WHY char* for outputs (not const)?
 * ──────────────────────────────────
 * Functions that return strings allocate NEW memory (via malloc).
 * The caller is responsible for freeing this memory using free_string().
 * It's non-const because the caller needs to pass it to free_string() later.
 *
 * MEMORY OWNERSHIP RULE:
 * ─────────────────────
 * C++ allocates (malloc) → Caller MUST free via free_string()
 * Failing to call free_string() = MEMORY LEAK
 */

#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Returns the length of string `s`.
 * Returns 0 if `s` is NULL.
 */
int str_length(const char* s);

/*
 * Returns a NEW reversed copy of string `s`.
 * Caller MUST free the returned pointer using free_string().
 * Returns NULL if `s` is NULL or if memory allocation fails.
 */
char* str_reverse(const char* s);

/*
 * Counts the number of vowels (a, e, i, o, u — both cases) in `s`.
 * Returns 0 if `s` is NULL.
 */
int count_vowels(const char* s);

/*
 * Returns a NEW uppercase copy of string `s`.
 * Caller MUST free the returned pointer using free_string().
 * Returns NULL if `s` is NULL or if memory allocation fails.
 */
char* to_uppercase(const char* s);

/*
 * Frees a string previously allocated by str_reverse() or to_uppercase().
 * Safe to call with NULL (no-op).
 *
 * WHY a dedicated free function?
 * ─────────────────────────────
 * Memory allocated by C++ (malloc) MUST be freed by C++ (free).
 * Rust cannot safely call free() on C++-allocated memory directly
 * because they might use different allocators. This function ensures
 * the same allocator handles both allocation and deallocation.
 */
void free_string(char* s);

#ifdef __cplusplus
}
#endif

#endif /* WRAPPER_H */
