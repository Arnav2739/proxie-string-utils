# Proxie String Utils — Rust FFI Wrapper for C++ Library

A safe Rust wrapper around a C++ string utilities library, demonstrating cross-language interoperability via FFI (Foreign Function Interface) and bindgen.

## Architecture

```
┌──────────────────┐     ┌─────────────────────┐     ┌──────────────────┐
│  Your Rust Code  │────▶│  Safe Wrapper Layer  │────▶│  C++ Library     │
│  (Public API)    │     │  (CString ↔ *c_char) │     │  (lib.cpp)       │
│                  │     │  (Error Handling)     │     │  (malloc/free)   │
└──────────────────┘     └─────────────────────┘     └──────────────────┘
```

## Prerequisites

| # | Tool | What It Does | Why You Need It |
|---|------|--------------|-----------------|
| 1 | **Rust** (rustup / cargo) | Language runtime & package manager | Compiles Rust code, manages crates (`cc`, `bindgen`), and runs `cargo test` |
| 2 | **C++ Compiler** (MSVC or g++ / clang++) | Compiles C++ code into machine code | Used by the `cc` crate in `build.rs` to compile `cpp/lib.cpp` into a static library |
| 3 | **LLVM** (libclang) | C/C++ parser engine | The `bindgen` crate uses this to parse `cpp/wrapper.h` and generate Rust FFI code |

### Installation

- **Rust** (1.70+) — Install via [rustup](https://rustup.rs/)
- **C++ Compiler**:
  - **Windows**: Install [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/) → select "Desktop development with C++"
  - **Linux**: `sudo apt install build-essential`
  - **macOS**: `xcode-select --install`
- **LLVM / libclang**:
  - **Windows**: Download from [LLVM Releases](https://github.com/llvm/llvm-project/releases) → select "Add LLVM to system PATH" during install
  - **Linux**: `sudo apt install libclang-dev`
  - **macOS**: Comes with Xcode Command Line Tools

## Build

```bash
# Clone the repository
git clone <repo-url>
cd proxie-string-utils

# Windows only: set LIBCLANG_PATH if LLVM is not on PATH
# (PowerShell — run once to set permanently)
# [System.Environment]::SetEnvironmentVariable("LIBCLANG_PATH", "C:\Program Files\LLVM\bin", "User")

# Build the project (compiles C++ → generates bindings → compiles Rust)
cargo build

# Run all tests
cargo test

# Run tests with output visible
cargo test -- --nocapture
```

## Usage

```rust
use proxie_string_utils::{string_length, reverse_string, vowel_count, uppercase};

fn main() {
    // Calculate string length
    let len = string_length("hello").unwrap();
    println!("Length: {}", len);  // 5

    // Reverse a string
    let reversed = reverse_string("hello").unwrap();
    println!("Reversed: {}", reversed);  // "olleh"

    // Count vowels
    let count = vowel_count("hello world").unwrap();
    println!("Vowels: {}", count);  // 3

    // Convert to uppercase
    let upper = uppercase("hello").unwrap();
    println!("Uppercase: {}", upper);  // "HELLO"
}
```

## Project Structure

```
proxie-string-utils/
├── build.rs              # Build orchestration (compiles C++ + generates bindings)
├── Cargo.toml            # Dependencies (cc, bindgen)
├── cpp/
│   ├── wrapper.h         # C-compatible header with extern "C" declarations
│   └── lib.cpp           # C++ implementation of string utilities
├── src/
│   └── lib.rs            # Safe Rust wrappers + comprehensive tests
└── README.md             # This file
```

## Design Decisions

### 1. Why `malloc`/`free` instead of `new`/`delete`?

`malloc`/`free` are C-standard functions that work predictably across FFI boundaries. `new`/`delete` are C++ specific — they may invoke constructors/destructors and use a different allocator. By sticking with `malloc`/`free`, we ensure the same allocator handles both allocation and deallocation, preventing heap corruption.

### 2. Memory Ownership Strategy

The "golden rule": **whoever allocates the memory must free it.** Our C++ functions allocate strings via `malloc`, so the Rust wrapper calls `free_string()` (which calls C's `free()`) to release that memory. The safe wrapper copies data into Rust-owned memory (`to_owned()`) *before* freeing the C++ pointer — preventing use-after-free.

### 3. Error Handling with `Result<T, StringUtilError>`

Instead of panicking on errors (which would crash the program), we return `Result` with a custom error enum. This is idiomatic Rust and gives callers full control over error recovery. The three error variants (`NullByte`, `InvalidUtf8`, `NullPointer`) cover every possible FFI failure mode.

### 4. Bindgen + `include!` Pattern

We use `bindgen` to auto-generate Rust FFI declarations from our C header. The `include!` macro inserts the generated code at compile time. This is the standard approach because the bindings file lives in `$OUT_DIR` (a build artifact directory), which is only accessible via `include!`. This also means bindings are always in sync with the header — change the C++ API, and the Rust bindings update automatically.

### 5. Unsafe Confinement

All `unsafe` blocks are confined to the internal `ffi` module and the private parts of our wrapper functions. The public API surface is 100% safe Rust — users never need to write `unsafe` code. This follows the Rust community's "safe abstraction over unsafe implementation" pattern.

## Tests

The test suite covers:

| Category | What's Tested |
|----------|---------------|
| **Basic Correctness** | All 4 functions with typical inputs |
| **Empty Strings** | All functions handle `""` correctly |
| **Single Characters** | `"a"`, `"Z"` — boundary cases |
| **Special Characters** | Numbers, punctuation, symbols |
| **Edge Cases** | Palindromes, no vowels, all vowels, already uppercase |
| **Error Handling** | Interior null bytes return proper error type |
| **Stress Testing** | 10,000+ character strings |
| **Integration** | reverse(reverse(x)) == x, uppercase preserves length |

Run tests with: `cargo test`
