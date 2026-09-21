add_library(device-service-lib
    src/domain/device_id.cpp
    src/domain/measurement.cpp
    src/domain/alarm.cpp
)

target_include_directories(device-service-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(device-service-lib
    PUBLIC
        cxx_std_20
)

add_executable(device-service
    src/main.cpp
)

target_link_libraries(device-service
    PRIVATE
        device-service-lib
)
