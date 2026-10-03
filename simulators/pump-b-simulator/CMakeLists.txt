add_executable(pump-b-simulator
    main.cpp
    pump_b_message_generator.cpp
)

target_include_directories(pump-b-simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(pump-b-simulator
    PRIVATE
        simulator-common
)

target_compile_features(pump-b-simulator
    PRIVATE
        cxx_std_20
)