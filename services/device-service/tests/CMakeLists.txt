find_package(GTest REQUIRED)

add_executable(device-service-tests 
    smoke_test.cpp

    domain/device_id_test.cpp
    domain/measurement_test.cpp
    domain/alarm_test.cpp
    domain/event_test.cpp
)

target_link_libraries(device-service-tests
    PRIVATE
        device-service-lib
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(device-service-tests)
