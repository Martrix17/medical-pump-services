find_package(GTest REQUIRED)

add_executable(pump-a-simulator-tests 
    message_generator_test.cpp
)

target_link_libraries(pump-a-simulator-tests 
    PRIVATE
        pump-a-simulator-lib
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(pump-a-simulator-tests)
