/*
 * build.rs — Build Script for Proxie String Utils
 *
 * WHAT IS build.rs?
 * ─────────────────
 * build.rs is Cargo's build script mechanism. It runs BEFORE your Rust code compiles.
 * We use it for two critical tasks:
 *   1. Compile our C++ code into a static library (using the `cc` crate)
 *   2. Generate Rust FFI bindings from our C header (using the `bindgen` crate)
 *
 * HOW THE BUILD PIPELINE WORKS:
 * ─────────────────────────────
 *   build.rs runs
 *     → cc compiles lib.cpp → produces libstringutils.a (static library)
 *     → bindgen reads wrapper.h → generates bindings.rs (Rust FFI declarations)
 *     → Cargo compiles src/lib.rs (which includes bindings.rs)
 *     → Cargo links everything together into the final binary
 *
 * WHAT IS cargo:rerun-if-changed?
 * ───────────────────────────────
 * Tells Cargo: "Only re-run build.rs if these files change."
 * Without this, build.rs would run on EVERY `cargo build`, even if nothing changed.
 */

fn main() {
    // ─────────────────────────────────────────────────────────────
    // STEP 1: Compile C++ source into a static library
    // ─────────────────────────────────────────────────────────────
    // The `cc` crate handles all the complexity of:
    //   - Finding the right C++ compiler for the current platform
    //   - Setting correct compiler flags
    //   - Producing a static library (.a on Linux/Mac, .lib on Windows)
    //   - Telling Cargo how to link against it
    cc::Build::new()
        .cpp(true)                            // Compile as C++ (not C)
        .file("cpp/lib.cpp")                  // Source file to compile
        .include("cpp")                       // Include path (so #include "wrapper.h" works)
        .flag_if_supported("-std=c++17")      // Use C++17 standard (if supported)
        .flag_if_supported("/std:c++17")      // MSVC equivalent for Windows
        .compile("stringutils");              // Output library name: libstringutils.a

    // ─────────────────────────────────────────────────────────────
    // STEP 2: Generate Rust FFI bindings from the C header
    // ─────────────────────────────────────────────────────────────
    // Bindgen reads wrapper.h and auto-generates Rust code like:
    //   extern "C" {
    //       pub fn str_length(s: *const c_char) -> c_int;
    //       pub fn str_reverse(s: *const c_char) -> *mut c_char;
    //       ...
    //   }
    let bindings = bindgen::Builder::default()
        .header("cpp/wrapper.h")              // Input: our C-compatible header
        .parse_callbacks(Box::new(            // Use default callbacks for type mapping
            bindgen::CargoCallbacks::new()
        ))
        .generate()                           // Generate the Rust bindings
        .expect("Unable to generate bindings");

    // Write the generated bindings to $OUT_DIR/bindings.rs
    // $OUT_DIR is a special Cargo-managed directory for build artifacts
    let out_path = std::path::PathBuf::from(std::env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    // ─────────────────────────────────────────────────────────────
    // STEP 3: Set up rebuild triggers
    // ─────────────────────────────────────────────────────────────
    // Only re-run this build script if the C++ files change
    println!("cargo:rerun-if-changed=cpp/wrapper.h");
    println!("cargo:rerun-if-changed=cpp/lib.cpp");
}
