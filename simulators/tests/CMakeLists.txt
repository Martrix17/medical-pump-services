find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(simulator-tests
    pump_a_message_generator_test.cpp
    pump_b_message_generator_test.cpp
    # tcp_server_test.cpp
    # tcp_session_test.cpp

    ../pump-a-simulator/pump_a_message_generator.cpp
    ../pump-b-simulator/pump_b_message_generator.cpp
)

target_include_directories(simulator-tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../pump-a-simulator
        ${CMAKE_CURRENT_SOURCE_DIR}/../pump-b-simulator
)

target_link_libraries(simulator-tests
    PRIVATE
        simulator-common
        GTest::gtest_main
        nlohmann_json::nlohmann_json
)

include(GoogleTest)

gtest_discover_tests(simulator-tests)