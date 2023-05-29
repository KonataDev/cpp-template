# cpp-template

<br/>

## Getting Started

### Prerequisites

- **[CMake v3.24+](https://github.com/BlurringShadow/stdsharp/blob/main/CMakeLists.txt#L1)** - required for building

- **C++ Compiler** - needs to support at least the **C++20** and **partial C++23** standard, i.e. _MSVC_, _GCC_, _Clang_. You could checkout [github workflow file](.github/workflows/build.yml) for suitable compilers.

  - Note that if you're using clang as compiler, [ld.lld](https://lld.llvm.org/) is required. Using [libc++](https://libcxx.llvm.org/) as compiler STL is recommended to ensure compatibility.

- **Vcpkg or Other Suitable Dependencies Manager** - this project uses vcpkg manifest to maintain dependencies. Checkout the
  [vcpkg.json](vcpkg.json) for required dependencies.

<br/>

### Installing

(Install guideline)

<br/>

### Building the project

Use cmake to build the project, checkout [github workflow file](.github/workflows/build.yml) for details.

<br/>

## License

This project is licensed under the [Unlicense](https://unlicense.org/) license - see the
[LICENSE](LICENSE) file for details
