find_package(asio REQUIRED)

add_library(simulator-common
    src/tcp_server.cpp
    src/tcp_session.cpp
)

target_include_directories(simulator-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(simulator-common
    PUBLIC
        asio::asio
)

target_compile_features(simulator-common
    PUBLIC
        cxx_std_20
)
