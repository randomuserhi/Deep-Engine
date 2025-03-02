- [ ] Reorganise to separate includes from source files
	- This is to match the standard of having "include" folder etc...
	- Also remove relative `#include`'s (have the compiler link to the include directory)

- [ ] Custom Allocator Support
	- [ ] `FixedSizeFreeList`
	- [ ] `PooledFixedSizeFreeList`
### Low Priority House Keeping

- [ ] Disable cpp exceptions 
- [ ] Replace STL containers with custom variants (in-house)
	- STL containers rely on exceptions, so disabling exceptions can pose an issue
	- STL containers are often not concurrent and we need concurrent versions
	- https://stackoverflow.com/a/44783029
	- https://stackoverflow.com/a/12938588

- [ ] Reorganise to use CMake

- [ ] Broader support for CPU Architectures - Macros for each architecture as well (Currently only supports 64 bit)
	- [ ] ARM
	- [ ] 32 Bit
	- [ ] WASM