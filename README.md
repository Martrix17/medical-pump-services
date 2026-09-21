### Medical Pump Services Dummy Project

This project serves as a dummy project for medical pumps.


# Conan build
conan profile detect

conan install . --build=missing -s build_type=Debug -of build/debug

# CMake presets
cmake --preset debug

cmake --build --preset debug

ctest --preset debug


# clang

clang-format -i services/device-service/src/main.cpp

clang-tidy services/device-service/src/main.cpp \
    -- \
    -std=c++20