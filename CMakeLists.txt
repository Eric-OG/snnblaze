cmake_minimum_required(VERSION 3.14)
project(snnblaze LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Library from your sources
add_library(neuralsim
    src/LIFNeuron.cpp
    src/NeuralNetwork.cpp
    src/SpikeMonitor.cpp
)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows: prevent overriding parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable
add_executable(test_neurons
    tests/test_LIFNeuron.cpp
)

target_link_libraries(test_neurons
    PRIVATE
    neuralsim
    gtest
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_neurons)
