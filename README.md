# File signatures console application and library

Utility generates a file with a list of binary signatures of the input file chunks.

:warning: Signature is an abstract term, which accumulates hash and checksum algorithms.

## Getting Started

Requires boost libraries and C++14 GNU compiler.

Project structure is inspired by 
[Canonical Project Structure](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html)
and [An Introduction to Modern CMake](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).

Code style is driven by [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html) with elements of
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md).

For more business and architectural details, please follow
[Architectural document](Documentation/Architecture.md).

### Prerequisites

```
install make
install cmake
install clang or gcc
install boost libraries
```

### Building

```
cd ${Project folder}
cmake . -B ${Build-folder-path}
cd ${Build-folder-path}
make
```

## Running the tests

Please, run tests on you platform to be sure that 

```
cd ${Build-folder-path}/build/bin
./tests
```

## Running utility

```
cd ${Build-folder-path}/build/bin
./SignatureGeneratorApplication --help
```

## Tested with

* MacOS Catalina 10.15.1
* cmake 3.15.5
* AppleClang 11
* Boost 1.71.0
* O2 optimisation flags
* Address, thread and memory sanitizers

## Supporting platforms/tools

* Unix OSs (Mac OS and Linux)
* Clang or GCC with C++14 support

## Known issues

### Multithreading support
For some platforms `std::thread::hardware_concurrency` can return `0` and reduce benefits of multi-CPU/core systems.

### Multithreading advantage
Current design focused on the processing a big amount of chunks and has an ouful performance 
for processing just couple of big chunks.

### Chunks minimal size
Minimal chunk size is 1 KiByte, which allows to guaranty mapping entire output file to the memory.

### Chunks maximum size
Maximum chunk size is not limited by UI, but limited by available RAM.
It can cause a crash due to luck of memory, depends on the memory map implementation and platform.

### Input file is less than chunks can cause crash or freeze
Due to Boost limitation passing files less than minimal chunk size (1KiByte) is nor valid for utility.
Tests now are working for 3 bytes files, but it's a risk zone.

### Microsoft VC compiler
cmake file are using GBU compiler flags and need to be updated for VC compilers.

Note: MinGW can work well, but has not been tested.

### Embedded support
Utility is focused on the server hardware with more than 1GB RAM and several CPUs/Cores.
Using utility with a less of RAM and in a one-thread enviroment is possible, but not optimal by design.

### Luck of exception OOP hierarchy
Using `std::excpetion` saves some development time, but need to be refactored in future.

#### Tests clean-up
E2E tests creates several files (up to 20 GB disk space) and are not removed after testing.

### Components are not implemented in own namespace
It's preferable to move components to own namespace, but on the next stage of development.

## Author and License

This project is under copyright and owned by Elisey Zamakhov, eliseyzamahov@gmail.com.