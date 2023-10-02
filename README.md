# blurhash-cxx
A fast blurhash implementation.

Uses C++20 and various other tactics to optimize for runtime performance.

TODO:
- [ ] Optimize Encode
- [ ] Create runtime variant of encode that can use the template for unrolling loops very aggressively.
- [ ] Optimize decode
- [ ] Create template variant of decode for the best optimization
- [ ] Create lookup tables for commonly used calculations.
- [ ] Constexpr EVERYTHING

- [ ] Add aggressive GCC flags (Personal standard)
- [ ] Expose C API so I can write more detailed and extensive testing
- [ ] Benchmark individual functions
