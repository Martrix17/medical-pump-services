find_package(nlohmann_json REQUIRED)

add_executable(pump-a-simulator
    main.cpp
    pump_a_message_generator.cpp
)

target_include_directories(pump-a-simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pump-a-simulator
    PRIVATE
        simulator-common
        nlohmann_json::nlohmann_json
)

target_compile_features(pump-a-simulator
    PRIVATE
        cxx_std_20
)
