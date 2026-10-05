find_package(GTest REQUIRED)

add_executable(device-service-tests 
    smoke_test.cpp

    adapter/pump_a_adapter_test.cpp
    adapter/pump_b_adapter_test.cpp
    
    communication/fixed_size_framer_test.cpp
    communication/line_framer_test.cpp

    concurrency/concurrent_queue_test.cpp
    concurrency/worker_test.cpp

    domain/device_id_test.cpp
    domain/measurement_test.cpp
    domain/alarm_test.cpp
    domain/event_test.cpp
    domain/device_status_test.cpp

    kafka/event_serializer_test.cpp
)

target_link_libraries(device-service-tests
    PRIVATE
        device-service-lib
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(device-service-tests)
