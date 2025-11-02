cmake_minimum_required(VERSION 3.14)
project(snnblaze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------
# Compiler optimization and OpenMP
# ----------------------------------
find_package(OpenMP REQUIRED)

# Add optimization and architecture-specific flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O3 -march=native -ffast-math)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/O2)
endif()

# Include OpenMP flags globally
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS}")

# ----------------------------------
# Include header files
# ----------------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----------------------------------
# Core library
# ----------------------------------
add_library(snnblaze
    src/LIFNeuron.cpp
    src/InputNeuron.cpp
    src/NeuralNetwork.cpp
    src/SpikeMonitor.cpp
    src/StateMonitor.cpp
)
set_target_properties(snnblaze PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(snnblaze PUBLIC OpenMP::OpenMP_CXX Python3::Python)

# ----------------------------------
# Python (pybind11)
# ----------------------------------
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

include(FetchContent)
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(pybind11)

pybind11_add_module(pysnnblaze src/bindings.cpp)
target_link_libraries(pysnnblaze PRIVATE snnblaze OpenMP::OpenMP_CXX)

# ----------------------------------
# GoogleTest
# ----------------------------------
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

file(GLOB TEST_SOURCES tests/*.cpp)
foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE snnblaze gtest gtest_main OpenMP::OpenMP_CXX)
    gtest_discover_tests(${test_name})
endforeach()
