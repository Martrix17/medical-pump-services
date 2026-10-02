find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)

add_library(device-service-lib
    src/adapter/pump_a_adapter.cpp
    src/adapter/pump_a_parser.cpp
    src/adapter/pump_b_adapter.cpp
    src/adapter/pump_b_parser.cpp
    src/adapter/pump_b_protocol.cpp

    src/common/alarm_parser.cpp
    src/common/binary_reader.cpp
    src/common/device_status_parser.cpp
    src/common/timestamp_parser.cpp

    src/communication/fixed_size_framer.cpp
    src/communication/line_framer.cpp

    src/concurrency/worker.cpp

    src/domain/device_id.cpp
    src/domain/measurement.cpp
    src/domain/alarm.cpp
    src/domain/device_status.cpp
    src/domain/state_enums.cpp
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
    src/communication/tcp_client.cpp
    src/event/console_event_sink.cpp
)

target_link_libraries(device-service
    PRIVATE
        device-service-lib
        asio::asio
)
