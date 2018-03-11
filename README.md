# Matrix Challenge
## By Siddharth Atre

### Building the project
This project uses the CMake build system. Create a build directory in the project and generate the build files of your choice. If compiling on a *nix based platform, I recommend using `Make` or `Ninja`. 

For example:

    cd MatrixTransform
    mkdir build
    cd build
    cmake -G "Unix Makefiles" ..

* CMake v3.10 or later is required.
* if using GCC, I recommend v7.0 or later
* if using Clang, I recommend v4.0 or later.
* Code is written against the C++11 standard.

There are two options that can be set in the build process:

* `USE_SIMD`, defaults to ON
* `USE_OpenMP`, defaults to ON

The former enables the use of SIMD vector processing in the matrix multiplication algorithm. The latter enables parellelism during the transpose and multiplication algorithms. Both are dependent on hardware support.

### Caveats
I tried to make the project as cross-platform as possible, but in order to implement the SIMD vector processing, I decided to use the Intel CPU-only intrinsics, rather than raw assembly. I decided to make this tradeoff so that I could complete the challenge in time. This means that the vector processing will not compile when building on AMD, ARM, or old (>5 years) Intel processors. I understand that this is not ideal for a general-purpose matrix library, but I believe it demonstrates my skill set. If I were implementing this for an actual industry use case, I would target whichever platform(s) the library was being built for.

In addition, there is currently a documented bug with the latest versions of CMake and Clang. The OpenMP support query in CMake always fails for Clang, which causes it to disable loop parallelism. For this reason, if you would like to see the optimal run time, I recommend using GCC. In addition, if compiling on MacOS, you will need to instal llvm (via Homebrew is best), as the clang supplied by default by Xcode is missing OpenMP.

### Running
There are two executables generated in the `bin` folder of the project: `CorrectnessTests` and `Profiler`. 

`CorrectnessTests` runs a suite of tests on the multiplication and transpose algorithms on the Matrix class for various data types. In order to verify the correctness of my algorithms, I included the popular Eigen linear algebra library. The results of each test are checked against Eigen.

`Profiler` runs the multiplication and transpose algorithms a series of times for each data type and reports the average run time for each data type.

### Further Improvements
I tried to optimize my class as much as possible given the time frame, however there are areas where it can be improved. The internal storage of the Matrix class has room for improvement related to its memory access time and cache optimization. In addition, the vector processing incurs a penalty as the rows need to be copied into contiguous memory in order to be used. If memory consumption is not a barrier, there are steps that can be taken to remedy this, such as duplicate storage in column and row major.

