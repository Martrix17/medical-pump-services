cmake_minimum_required(VERSION 4.0)

project(medical-pump-services
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTING "Build tests" ON)

add_subdirectory(services/device-service)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(services/device-service/tests)
endif()

option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)

if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()