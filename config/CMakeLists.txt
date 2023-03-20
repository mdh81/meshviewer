add_library(
    config 
    ConfigReader.cpp
)

add_custom_target(
    cfgFileCopy ALL
    ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/defaults.cfg ${CMAKE_CURRENT_BINARY_DIR}/
)

add_dependencies(
    config
    cfgFileCopy
)

enable_testing()
add_subdirectory(tests)
