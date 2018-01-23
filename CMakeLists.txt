set(PROJECT_NAME MatrixChallenge)

cmake_minimum_required(VERSION 3.5)
project(MatrixChallenge CXX)

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CMAKE_CXX_STANDARD 14) # C++14 because it's 2017...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #...useful for some IDEs and text editors.
set(CMAKE_BUILD_TYPE Debug) # Change to Release if profiling.
set(THREADS_PREFER_PTHREAD ON)
set(CMAKE_MACOSX_RPATH 1) # Prevents warnings on MacOS
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

# GLM is used to verify test results.
add_subdirectory(glm)

# Add the source directory
add_subdirectory(source)