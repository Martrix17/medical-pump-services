find_package(asio REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(pump-a-simulator-lib
    src/message_generator.cpp
)

target_include_directories(pump-a-simulator-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(pump-a-simulator-lib
    PUBLIC
        cxx_std_20
)

target_link_libraries(pump-a-simulator-lib
    PUBLIC
        nlohmann_json::nlohmann_json
)

add_executable(pump-a-simulator
    src/main.cpp
    src/tcp_server.cpp
    src/session.cpp
)

target_link_libraries(pump-a-simulator
    PRIVATE
        pump-a-simulator-lib
        asio::asio
)
