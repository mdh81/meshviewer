add_executable(
    configuration_reader_test
    configuration_reader_test.cpp
)

target_include_directories(
    configuration_reader_test
    PUBLIC
    ../
    ../../
)

target_link_libraries(
    configuration_reader_test
    PUBLIC
    gtest_main
    configuration
    common
)
