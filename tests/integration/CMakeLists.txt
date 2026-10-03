find_package(GTest REQUIRED)
find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(device-service-integration-tests
    pump_a_integration_test.cpp
    pump_b_integration_test.cpp
)

target_include_directories(device-service-integration-tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device-service-integration-tests
    PRIVATE
        device-service-lib
        GTest::gtest_main
        asio::asio
        nlohmann_json::nlohmann_json
)

include(GoogleTest)

gtest_discover_tests(device-service-integration-tests)