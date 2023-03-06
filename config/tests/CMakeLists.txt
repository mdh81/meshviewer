add_executable(
    config_reader_test 
    config_reader_test.cpp
)

target_include_directories(
    config_reader_test 
    PUBLIC
    ../
)

target_link_libraries(
    config_reader_test
    PUBLIC
    gtest_main
    config
)
