add_library(
    configuration
    ConfigurationReader.cpp
)

add_custom_target(
    cfgFileCopy ALL
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/defaults.cfg ${CMAKE_CURRENT_BINARY_DIR}/
)

target_include_directories(
    configuration
    PUBLIC
    ../common
    .
)

target_link_libraries(
    configuration
    PUBLIC
    common
)

add_dependencies(
    configuration
    cfgFileCopy
)

if (enableTesting)
    enable_testing()
    add_subdirectory(tests)
endif()
