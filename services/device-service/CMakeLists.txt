find_package(nlohmann_json REQUIRED)

add_library(device-service-lib
    src/domain/device_id.cpp
    src/domain/measurement.cpp
    src/domain/alarm.cpp

    src/adapter/pump_a_adapter.cpp
    src/adapter/pump_a_parser.cpp

    src/common/alarm_parser.cpp
    src/common/device_status_parser.cpp
    src/common/timestamp_parser.cpp
)

target_include_directories(device-service-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(device-service-lib
    PUBLIC
        cxx_std_20
)

target_link_libraries(device-service-lib
    PUBLIC
        nlohmann_json::nlohmann_json
)

add_executable(device-service
    src/main.cpp
)

target_link_libraries(device-service
    PRIVATE
        device-service-lib
        # nlohmann_json::nlohmann_json
)
