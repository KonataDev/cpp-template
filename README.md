[![Build](https://github.com/BlurringShadow/stdsharp/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/BlurringShadow/stdsharp/actions/workflows/build.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/f08b08ddd5e146c69b39ac5001f06c6a)](https://www.codacy.com/gh/BlurringShadow/stdsharp/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=BlurringShadow/stdsharp&amp;utm_campaign=Badge_Grade)

# stdsharp

Contains commonly used c++ code that not impl or well impl in other libs. This project aim to supplement the c++ standard library with fundamental features. Headers are arranged in accordance with the standard library structure.

## Getting Started

### Prerequisites

* **CMake v3.15+**

* **C++ Compiler** - needs to support at least the **C++20** standard, i.e. *MSVC*, *GCC*, *Clang*

* **Vcpkg or Other Suitable Dependencies Manager** - this project use vcpkg manifest to maintain dependencies. check the
  [vcpkg.json](https://github.com/BlurringShadow/stdsharp/blob/master/vcpkg.json) for required dependencies

### Installing

Clone the project to your source dir or add this lib to your custom vcpkg registry

## Building the project

Use cmake to build the project

## License

This project is licensed under the [Unlicense](https://unlicense.org/) - see the
[LICENSE](LICENSE) file for details
