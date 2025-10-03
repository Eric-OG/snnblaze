cmake_minimum_required(VERSION 3.14)
project(snnblaze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Library from your sources
add_library(snnblaze
    src/LIFNeuron.cpp
    src/NeuralNetwork.cpp
    src/SpikeMonitor.cpp
    src/StateMonitor.cpp
)

set_target_properties(snnblaze PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Include python headers
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# ---------------------------
# Fetch pybind11
# ---------------------------
include(FetchContent)
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/heads/master.zip
)
FetchContent_MakeAvailable(pybind11)

# ---------------------------
# Python extension module
# ---------------------------
# Assuming you create src/bindings.cpp for your pybind11 bindings
pybind11_add_module(pysnnblaze src/bindings.cpp)

# Link with your library
target_link_libraries(pysnnblaze PRIVATE snnblaze)

# ---------------------------
# GoogleTest
# ---------------------------
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Windows fix
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# ---------------------------
# Collect and build all test files in tests/
# ---------------------------
file(GLOB TEST_SOURCES tests/*.cpp)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE) # e.g. test_LIFNeuron
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE snnblaze gtest gtest_main)
    gtest_discover_tests(${test_name})
endforeach()